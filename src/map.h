#pragma once
#include "gba.h"

#define TILE_GRASS      1
#define TILE_DIRT       2
#define TILE_PATH       3
#define TILE_WATER      4
#define TILE_TREE       5
#define TILE_FENCE      6
#define TILE_WALL       7
#define TILE_FLOOR      8
#define TILE_TILLED     9
#define TILE_WATERED    10
#define TILE_PLANTED    11

void map_init(void);
u8 map_tile_at(int wx, int wy);
int map_is_solid(u8 tile_id);
void map_till(int wx, int wy);
void map_water(int wx, int wy);
void map_dry_fields(void);
void map_plant(int wx, int wy);