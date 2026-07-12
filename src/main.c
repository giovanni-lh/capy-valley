#include "gba.h"
#include "input.h"
#include "map.h"
#include "player.h"
#include "clock.h"
#include "hud.h"
#include "npc.h"
#include "dialogue.h"

int main(void){
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
    player_init(&p, 112, 72);

    NpcSchedule sched1 = {
        {{6, 48, 72}, {7, 160, 88}, {8, 48, 72}},
        3
    };

    Npc npc1;
    npc_init(&npc1, 160, 88, 1, 1);

    Clock clk;
    clock_init(&clk);
    hud_init();

    int dlg_open = 0;

    while (1) {
        vsync();
        input_update();
        player_update(&p);
        clock_tick(&clk);
        hud_update(&clk);
        npc_schedule_apply(&npc1, &sched1, clock_hour(&clk));

        int dx = (p.wx + 8) - (npc1.wx + 8);
        int dy = (p.wy + 8) - (npc1.wy + 8);
        int near_npc = (dx > -24 && dx < 24 && dy > -24 && dy < 24);

        if (!dlg_open && near_npc && key_pressed(KEY_A)) {
            dlg_open = 1;
            dlg_show("HELLO TRAVELER");
        } else if (dlg_open && key_pressed(KEY_A)) {
            dlg_open = 0;
            dlg_hide();
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