#pragma once
#include "gba.h"

typedef struct {
    int wx;
    int wy;
    int oam_idx;
    int pal_bank;
    int hearts;
} Npc;

#define NPC_MAX_WAYPOINTS 8

typedef struct {
    int hour;
    int wx;
    int wy;
} Waypoint;

typedef struct {
    Waypoint waypoints[NPC_MAX_WAYPOINTS];
    int count;
} NpcSchedule;

void npc_init(Npc *n, int wx, int wy, int oam_idx, int pal_bank);
void npc_draw(const Npc *n, int cam_x, int cam_y);
void npc_hide(const Npc *n);
void npc_schedule_apply(Npc *n, const NpcSchedule *s, int hour);