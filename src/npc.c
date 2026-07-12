#include "npc.h"

void npc_init(Npc *n, int wx, int wy, int oam_idx, int pal_bank) {
    n->wx = wx;
    n->wy = wy;
    n->oam_idx = oam_idx;
    n->pal_bank = pal_bank;
}

void npc_draw(const Npc *n, int cam_x, int cam_y) {
    int sx = n->wx - cam_x;
    int sy = n->wy - cam_y;

    OAM[n->oam_idx].attr0 = ATTR0_Y(sy) | ATTR0_NORMAL | ATTR0_CLR4 | ATTR0_SQUARE;
    OAM[n->oam_idx].attr1 = ATTR1_X(sx) | ATTR1_SZ16;
    OAM[n->oam_idx].attr2 = ATTR2_TILE(0) | ATTR2_PRIO(0) | ATTR2_PALETTE(n->pal_bank);
}

void npc_hide(const Npc *n) {
    OAM[n->oam_idx].attr0 = ATTR0_HIDDEN;
}

void npc_schedule_apply(Npc *n, const NpcSchedule *s, int hour) {
    int best = 0;
    for (int i = 0; i < s->count; i++) {
        if (s->waypoints[i].hour <= hour) best = i;
    }
    n->wx = s->waypoints[best].wx;
    n->wy = s->waypoints[best].wy;
}