#include "gba.h"
#include "input.h"
#include "map.h"
#include "player.h"

int main(void){
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;

    REG_BG2CNT = BG_CBB(0) | BG_SBB(28) | BG_4BPP | BG_SIZE_64x32 | BG_PRIO(3);

    map_init();

    for (int i = 0; i < 128; i++) OAM[i].attr0 = ATTR0_HIDDEN;

    OBJ_PALETTE[0] = RGB15(0, 0, 0);
    OBJ_PALETTE[1] = RGB15(26, 18, 9);

    for (int i = 0; i < 4 * 8; i++) OBJ_TILES[i] = 0x11111111u;

    OAM[0].attr2 = ATTR2_TILE(0) | ATTR2_PRIO(0) | ATTR2_PALETTE(0);
    OAM[0].pad = 0;

    Player p;
    player_init(&p, 112, 72);

    while (1) {
        vsync();
        input_update();
        player_update(&p);

        int cam_x = p.wx - 122;
        int cam_y = p.wy - 72;
        if (cam_x < 0) cam_x = 0;
        if (cam_x > 80) cam_x = 80;
        if (cam_y < 0) cam_y = 0;
        if (cam_y > 80) cam_y = 80;

        REG_BG2HOFS = (u16)cam_x;
        REG_BG2VOFS = (u16)cam_y;

        OAM[0].attr0 = ATTR0_Y(p.wy - cam_y) | ATTR0_NORMAL | ATTR0_CLR4 | ATTR0_SQUARE;
        OAM[0].attr1 = ATTR1_X(p.wx - cam_x) | ATTR1_SZ16;
    }
}