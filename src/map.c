#include "map.h"

static const u16 tile_colors[14] = {
    RGB15(0, 0, 0),     /* 0: reserved */
    RGB15(6, 20, 3),    /* 1: GRASS */
    RGB15(18, 12, 4),   /* 2: DIRT */
    RGB15(16, 16, 16),  /* 3: PATH */
    RGB15(3, 14, 26),   /* 4: WATER */
    RGB15(2, 12, 2),    /* 5: TREE */
    RGB15(20, 14, 6),   /* 6: FENCE */
    RGB15(10, 10, 10),  /* 7: WALL */
    RGB15(22, 18, 12),  /* 8: FLOOR */
    RGB15(10, 7, 2),    /* 9: TILLED */
    RGB15(7, 9, 5),     /* 10: WATERED */
    RGB15(6, 11, 3),    /* 11: PLANTED */
    RGB15(4, 24, 4),    /* 12: GROWN */
    RGB15(18, 16, 14),  /* 13: ORE */
};

static u8 farm_state[15][20];
static u8 crop_age[15][20];

/* 20 cols x 15 rows; each cell = 16x16 px = one player sprite */
/* tile IDs: 1=GRASS 2=DIRT 3=PATH 4=WATER 5=TREE 6=FENCE 7=WALL 8=FLOOR */
static const u8 farm_map[15][20] = {
    {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},
    {5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5},
    {5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5},
    {5, 1, 7, 7, 7, 7, 7, 1, 6, 6, 6, 6, 6, 6, 1, 1, 1, 1, 1, 5},
    {5, 1, 7, 8, 8, 8, 7, 1, 6, 8, 8, 8, 8, 6, 1, 1, 1, 1, 1, 5},
    {5, 1, 7, 7, 3, 7, 7, 1, 6, 6, 3, 6, 6, 6, 1, 1, 1, 1, 1, 5},
    {5, 1, 1, 1, 3, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 5},
    {5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 5},
    {5, 1, 1, 1, 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5},
    {5, 1, 1, 1, 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5},
    {5, 1, 1, 1, 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5},
    {5, 1, 1, 1, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5},
    {5, 1, 1, 1, 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5},
    {5, 1, 1, 1, 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5},
    {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5}
};

/* write tile tid to both 8x8 hardware tiles that form meta-tile (mc, mr) */
static void map_patch(int mc, int mr, u16 tid) {
    for (int dr = 0; dr < 2; dr++) {
        for (int dc = 0; dc < 2; dc++) {
            int tr = mr * 2 + dr;
            int tc = mc * 2 + dc;
            volatile u16 *sbb = (tc < 32) ? BG_SCREENBLOCK(28) : BG_SCREENBLOCK(29);
            int scol = (tc < 32) ? tc : tc - 32;
            sbb[tr * 32 + scol] = tid;
        }
    }
}

static void map_load(void) {
    for (int mr = 0; mr < 15; mr++) {
        for (int mc = 0; mc < 20; mc++) {
            map_patch(mc, mr, farm_map[mr][mc]);
        }
    }
}

void map_init(void) {
    for (int i = 0; i < 14; i++) {
        BG_PALETTE[i] = tile_colors[i];
    }
    for (int t = 1; t <= 13; t++) {
        volatile u32 *tile = BG_CHARBLOCK(0) + t * 8;
        u32 word = (u32)t * 0x11111111u;
        for (int r = 0; r < 8; r++) tile[r] = word;
    }
    map_load();

    static const int ore[][2] = {
        {14, 8}, {16, 8},
        {15, 9},
        {14, 10}, {16, 10},
        {13, 12}, {15, 12},
        {14, 13},
    };
    for (int i = 0; i < 8; i++) {
        int mc = ore[i][0], mr = ore[i][1];
        farm_state[mr][mc] = TILE_ORE;
        map_patch(mc, mr, TILE_ORE);
    }
}

u8 map_tile_at(int wx, int wy) {
    int mc = wx / 16;
    int mr = wy / 16;
    if (mc < 0 || mc >= 20 || mr < 0 || mr >= 15) return TILE_TREE;
    if (farm_state[mr][mc] != 0) return farm_state[mr][mc];
    return farm_map[mr][mc];
}

int map_is_solid(u8 tile_id) {
    return tile_id == TILE_WATER ||
           tile_id == TILE_TREE  ||
           tile_id == TILE_FENCE ||
           tile_id == TILE_WALL  ||
           tile_id == TILE_ORE;
}

void map_till(int wx, int wy) {
    int mc = wx / 16;
    int mr = wy / 16;
    if (mc < 0 || mc >= 20 || mr < 0 || mr >= 15) return;
    if (farm_map[mr][mc] != TILE_DIRT) return;
    farm_state[mr][mc] = TILE_TILLED;
    map_patch(mc, mr, TILE_TILLED);
}

void map_water(int wx, int wy) {
    int mc = wx / 16;
    int mr = wy / 16;
    if (mc < 0 || mc >= 20 || mr < 0 || mr >= 15) return;
    if (farm_state[mr][mc] != TILE_TILLED) return;
    farm_state[mr][mc] = TILE_WATERED;
    map_patch(mc, mr, TILE_WATERED);
}

void map_dry_fields(void) {
    for (int mr = 0; mr < 15; mr++) {
        for (int mc = 0; mc < 20; mc++) {
            if (farm_state[mr][mc] != TILE_WATERED) continue;
            farm_state[mr][mc] = TILE_TILLED;
            map_patch(mc, mr, TILE_TILLED);
        }
    }
}

void map_plant(int wx, int wy) {
    int mc = wx / 16;
    int mr = wy / 16;
    if (mc < 0 || mc >= 20 || mr < 0 || mr >= 15) return;
    if (farm_state[mr][mc] != TILE_WATERED) return;
    farm_state[mr][mc] = TILE_PLANTED;
    crop_age[mr][mc] = 0;
    map_patch(mc, mr, TILE_PLANTED);
}

void map_grow_crops(void) {
    for (int mr = 0; mr < 15; mr++) {
        for (int mc = 0; mc < 20; mc++) {
            if (farm_state[mr][mc] != TILE_PLANTED) continue;
            crop_age[mr][mc]++;
            if (crop_age[mr][mc] < 4) continue;
            farm_state[mr][mc] = TILE_GROWN;
            map_patch(mc, mr, TILE_GROWN);
        }
    }
}

void map_harvest(int wx, int wy) {
    int mc = wx / 16;
    int mr = wy / 16;
    if (mc < 0 || mc >= 20 || mr < 0 || mr >= 15) return;
    if (farm_state[mr][mc] != TILE_GROWN) return;
    farm_state[mr][mc] = TILE_TILLED;
    crop_age[mr][mc] = 0;
    map_patch(mc, mr, TILE_TILLED);
}

void map_mine(int wx, int wy) {
    int mc = wx / 16;
    int mr = wy / 16;
    if (mc < 0 || mc >= 20 || mr < 0 || mr >= 15) return;
    if (farm_state[mr][mc] != TILE_ORE) return;
    farm_state[mr][mc] = 0;
    map_patch(mc, mr, farm_map[mr][mc]);
}

void map_wilt_crops(void) {
    for (int mr = 0; mr < 15; mr++) {
        for (int mc = 0; mc < 20; mc++) {
            u8 s = farm_state[mr][mc];
            if (s != TILE_PLANTED && s != TILE_GROWN) continue;
            farm_state[mr][mc] = TILE_TILLED;
            crop_age[mr][mc] = 0;
            map_patch(mc, mr, TILE_TILLED);
        }
    }
}