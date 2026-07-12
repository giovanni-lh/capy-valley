#include "gba.h"
#include "input.h"
#include "map.h"
#include "player.h"
#include "clock.h"
#include "hud.h"
#include "npc.h"
#include "dialogue.h"

#define FISH_IDLE 0
#define FISH_WAIT 1
#define FISH_BITE 2
#define FISH_RESULT 3

static u32 rng_state = 1;
static int rng_range(int lo, int hi) {
    rng_state = rng_state * 1664525u + 1013904223u;
    return lo + (int)((rng_state >> 16) % (unsigned)(hi - lo + 1));
}

int main(void) {
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;

    REG_BG0CNT = BG_CBB(1) | BG_SBB(30) | BG_4BPP | BG_SIZE_32x32 | BG_PRIO(0);
    REG_BG1CNT = BG_CBB(2) | BG_SBB(31) | BG_4BPP | BG_SIZE_32x32 | BG_PRIO(1);
    REG_BG2CNT = BG_CBB(0) | BG_SBB(28) | BG_4BPP | BG_SIZE_64x32 | BG_PRIO(3);

    map_init();
    dlg_init();

    for (int i = 0; i < 128; i++) OAM[i].attr0 = ATTR0_HIDDEN;

    OBJ_PALETTE[0] = RGB15(0, 0, 0);
    OBJ_PALETTE[1] = RGB15(26, 18, 9);
    OBJ_PALETTE[16] = RGB15(0, 0, 0);
    OBJ_PALETTE[17] = RGB15(10, 18, 28);

    for (int i = 0; i < 4 * 8; i++) OBJ_TILES[i] = 0x11111111u;

    OAM[0].attr2 = ATTR2_TILE(0) | ATTR2_PRIO(0) | ATTR2_PALETTE(0);
    OAM[0].pad = 0;

    Player p;
    player_init(&p, 64, 112); /* col 4, row 7 - main path */

    NpcSchedule sched1 = {
        {{6, 48, 96}, {7, 160, 96}, {8, 48, 96}},
        3
    };

    Npc npc1;
    npc_init(&npc1, 48, 96, 1, 1); /* col 3, row 6 - grass by house door */

    Clock clk;
    clock_init(&clk);
    hud_init();

    int dlg_open = 0;
    int prev_day = clk.day;
    int prev_season = clk.season;
    int fish_state = FISH_IDLE;
    int fish_timer = 0;

    /* facing offsets: 0=up 1=down 2=Left 3=right */
    static const int face_dx[] = { 0, 0, -16, 16};
    static const int face_dy[] = {-16, 16, 0, 0};

    while (1) {
        vsync();
        input_update();
        if (fish_state == FISH_IDLE) player_update(&p);
        clock_tick(&clk);
        if (clk.day != prev_day) {
            map_dry_fields();
            map_grow_crops();
            if (clk.season != prev_season) {
                map_wilt_crops();
                prev_season = clk.season;
            }
            prev_day = clk.day;
        }
        hud_update(&clk);
        npc_schedule_apply(&npc1, &sched1, clock_hour(&clk));

        /* tile directly in front of the player */
        int face_wx = p.wx + face_dx[p.facing];
        int face_wy = p.wy + face_dy[p.facing];

        int near_npc = (face_wx == npc1.wx && face_wy == npc1.wy);
        int near_water = (map_tile_at(face_wx, face_wy) == TILE_WATER);

        /* fishing timer - runs every frame regardless of step */
        if (fish_state == FISH_WAIT && --fish_timer == 0) {
            fish_state = FISH_BITE;
            fish_timer = 60;
            dlg_show("! BITE !");
        } else if (fish_state == FISH_BITE && --fish_timer == 0) {
            fish_state = FISH_RESULT;
            dlg_show("IT GOT AWAY...");
        }

        /* interactions only when player is not mid-step */
        if (p.step_frames == 0) {
            if (fish_state == FISH_BITE && key_pressed(KEY_A)) {
                fish_state = FISH_RESULT;
                dlg_show("CAUGHT A FISH!");
            } else if (fish_state == FISH_RESULT && key_pressed(KEY_A)) {
                fish_state = FISH_IDLE;
                dlg_hide();
            } else if (fish_state > FISH_IDLE) {
                /* FISH_WAIT - swallow all other A presses */
            } else if (dlg_open && key_pressed(KEY_A)) {
                dlg_open = 0;
                dlg_hide();
            } else if (!dlg_open && near_npc && key_pressed(KEY_A)) {
                const char *line;
                if (npc1.hearts < 3) line = "HELLO TRAVELER";
                else if (npc1.hearts < 6) line = "GOOD DAY FRIEND";
                else line = "WELCOME BACK";
                npc1.hearts++;
                if (npc1.hearts > 10) npc1.hearts = 10;
                dlg_open = 1;
                dlg_show(line);
            } else if (!dlg_open && near_water && key_pressed(KEY_A)) {
                fish_state = FISH_WAIT;
                fish_timer = rng_range(60, 180);
                dlg_show("FISHING...");
            } else if (!dlg_open && key_pressed(KEY_A)) {
                map_water(face_wx, face_wy);
            }

            if (fish_state == FISH_WAIT || fish_state == FISH_BITE) {
            } else if (fish_state == FISH_IDLE) {
                if (key_pressed(KEY_B)) map_till(face_wx, face_wy);
                if (key_pressed(KEY_SELECT)) map_plant(face_wx, face_wy);
                if (key_pressed(KEY_START)) map_harvest(face_wx, face_wy);
            }

            int cam_x = p.wx - 112;
            int cam_y = p.wy - 72;
            if (cam_x < 0) cam_x = 0;
            if (cam_x > 80) cam_x = 80;
            if (cam_y < 0) cam_y = 0;
            if (cam_y > 80) cam_y = 80;

            REG_BG2HOFS = (u16)cam_x;
            REG_BG2VOFS = (u16)cam_y;

            OAM[0].attr0 = ATTR0_Y(p.wy - cam_y) | ATTR0_NORMAL | ATTR0_CLR4 | ATTR0_SQUARE;
            OAM[0].attr1 = ATTR1_X(p.wx - cam_x) | ATTR1_SZ16;
            npc_draw(&npc1, cam_x, cam_y);
        }
    }
}