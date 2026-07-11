#include "player.h"
#include "map.h"
#include "input.h"

#define MAP_W_PX 320
#define MAP_H_PX 240
#define SPR_SIZE 16

static int solid_at(int wx, int wy) {
    return  map_is_solid(map_tile_at(wx,                wy               )) ||
            map_is_solid(map_tile_at(wx + SPR_SIZE - 1, wy               )) ||
            map_is_solid(map_tile_at(wx,                wy + SPR_SIZE - 1)) ||
            map_is_solid(map_tile_at(wx + SPR_SIZE - 1, wy + SPR_SIZE - 1));
}

void player_init(Player *p, int start_wx, int start_wy) {
    p->wx = start_wx;
    p->wy = start_wy;
}

void player_update(Player *p) {
    int nx = p->wx;
    int ny = p->wy;

    if (key_held(KEY_UP))      ny -= 2;
    if (key_held(KEY_DOWN))    ny += 2;
    if (key_held(KEY_LEFT))    nx -= 2;
    if (key_held(KEY_RIGHT))   nx += 2;

    if (nx < 0) nx = 0;
    if (nx > MAP_W_PX - SPR_SIZE) nx = MAP_W_PX - SPR_SIZE;
    if (ny < 0) ny = 0;
    if (ny > MAP_H_PX - SPR_SIZE) ny = MAP_H_PX - SPR_SIZE;

    if (!solid_at(nx, p->wy)) p->wx = nx;
    if (!solid_at(p->wx, ny)) p->wy = ny;
}