#include "player.h"
#include "map.h"
#include "input.h"

#define MAP_W_PX 320
#define MAP_H_PX 240
#define SPR_SIZE 16
#define STEP_PX 16   /* one meta-tile per step */
#define STEP_FRAMES 8 /* 8 frames x 2 px = 16 px */

static int solid_at(int wx, int wy) {
    return map_is_solid(map_tile_at(wx,                  wy)) ||
           map_is_solid(map_tile_at(wx + SPR_SIZE - 1,   wy)) ||
           map_is_solid(map_tile_at(wx,                  wy + SPR_SIZE - 1)) ||
           map_is_solid(map_tile_at(wx + SPR_SIZE - 1,   wy + SPR_SIZE - 1));
}

void player_init(Player *p, int start_wx, int start_wy) {
    p->wx = start_wx;
    p->wy = start_wy;
    p->facing = 1;
    p->step_frames = 0;
}

void player_update(Player *p) {
    if (p->step_frames > 0) {
        static const int sdx[] = { 0, 0, -2, 2 };
        static const int sdy[] = { -2, 2, 0, 0 };
        p->wx += sdx[p->facing];
        p->wy += sdy[p->facing];
        p->step_frames--;
        return;
    }

    int dir = -1;
    if (key_held(KEY_UP))    dir = 0;
    else if (key_held(KEY_DOWN))  dir = 1;
    else if (key_held(KEY_LEFT))  dir = 2;
    else if (key_held(KEY_RIGHT)) dir = 3;

    if (dir < 0) return;
    p->facing = dir;

    static const int ddx[] = { 0, 0, -STEP_PX, STEP_PX };
    static const int ddy[] = { -STEP_PX, STEP_PX, 0, 0 };
    int dest_wx = p->wx + ddx[dir];
    int dest_wy = p->wy + ddy[dir];

    if (dest_wx < 0) dest_wx = 0;
    if (dest_wx > MAP_W_PX - SPR_SIZE) dest_wx = MAP_W_PX - SPR_SIZE;
    if (dest_wy < 0) dest_wy = 0;
    if (dest_wy > MAP_H_PX - SPR_SIZE) dest_wy = MAP_H_PX - SPR_SIZE;

    if (solid_at(dest_wx, dest_wy)) return;
    p->step_frames = STEP_FRAMES;
}