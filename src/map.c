#include "map.h"

static const u16 tile_colors[13] = {
    RGB15(0, 0, 0),       /* 0:  reserved */
    RGB15(6, 20, 3),      /* 1:  GRASS */
    RGB15(18, 12, 4),     /* 2:  DIRT */
    RGB15(16, 16, 16),    /* 3:  PATH */
    RGB15(3, 14, 26),     /* 4:  WATER */
    RGB15(2, 12, 2),      /* 5:  TREE */
    RGB15(20, 14, 6),     /* 6:  FENCE */
    RGB15(10, 10, 10),    /* 7:  WALL */
    RGB15(22, 18, 12),    /* 8:  FLOOR */
    RGB15(10, 7, 2),      /* 9:  TILLED */
    RGB15(7, 9, 5),       /* 10: WATERED */
    RGB15(6, 11, 3),      /* 11: PLANTED */
    RGB15(4, 24, 4),      /* 12: GROWN */
};

static u8 farm_state[30][40];
static u8 crop_age[30][40];

/* tile IDs: 1=GRASS 2=DIRT 3=PATH 4=WATER 5=TREE 6=FENCE 7=WALL 8=FLOOR */
static const u8 farm_map[30][40] = {
    /* r0*/ {5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5},
    /* r1*/ {5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5},
    /* r2*/ {5,5,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,5,5},
    /* r3*/ {5,5,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,5,5},
    /* r4*/ {5,5,1,7,7,7,7,7,7,7,1,1,1,1,1,1,1,1,6,6,6,6,6,6,6,6,6,6,1,1,1,1,5,5,5,5,5,5,5,5},
    /* r5*/ {5,5,1,7,8,8,8,8,8,7,1,1,1,1,1,1,1,1,6,8,8,8,8,8,8,8,8,6,1,1,1,1,5,5,5,5,5,5,5,5},
    /* r6*/ {5,5,1,7,8,8,8,8,8,7,1,1,1,1,1,1,1,1,6,8,8,8,8,8,8,8,8,6,1,1,1,1,5,5,5,5,5,5,5,5},
    /* r7*/ {5,5,1,7,8,8,8,8,8,7,1,1,1,1,1,1,1,1,6,8,8,8,8,8,8,8,8,6,1,1,1,1,5,5,5,5,5,5,5,5},
    /* r8*/ {5,5,1,7,8,8,8,8,8,7,1,1,1,1,1,1,1,1,6,8,8,8,8,8,8,8,8,6,1,1,1,1,5,5,5,5,5,5,5,5},
    /* r9*/ {5,5,1,7,7,7,3,7,7,7,1,1,1,1,1,1,1,1,6,6,6,6,6,6,6,6,6,6,1,1,1,1,5,5,5,5,5,5,5,5},
    /*r10*/ {5,5,1,1,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,5,5,5,5,5,5,5,5},
    /*r11*/ {5,5,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,5,5,5,5,5,5,5,5},
    /*r12*/ {5,5,1,1,1,1,1,3,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,5,5,5,5,5,5,5,5},
    /*r13*/ {5,5,1,1,1,1,1,3,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,5,5,5,5,5,5,5,5},
    /*r14*/ {5,5,1,1,1,1,1,3,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,5,5,5,5,5,5,5,5},
    /*r15*/ {5,5,1,1,1,1,1,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,5,5,5,5,5,5,5,5},
    /*r16*/ {5,5,1,1,1,1,1,3,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,5,5,5,5,5,5,5,5},
    /*r17*/ {5,5,1,1,1,1,1,3,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,5,5,5,5,5,5,5,5},
    /*r18*/ {5,5,1,1,1,1,1,3,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,5,5,5,5,5,5,5,5},
    /*r19*/ {5,5,1,1,1,1,1,3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,5,5,5,5,5,5,5,5},
    /*r20*/ {5,5,1,1,1,1,1,3,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,5,5,5,5,5,5,5,5},
    /*r21*/ {5,5,1,1,1,1,1,3,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,5,5,5,5,5,5,5,5},
    /*r22*/ {5,5,1,1,1,1,1,3,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,5,5,5,5,5,5,5,5},
    /*r23*/ {5,5,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,5,5,5,5,5,5,5,5},
    /*r24*/ {5,5,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,5,5,5,5,5,5,5,5},
    /*r25*/ {5,5,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,5,5,5,5,5,5,5,5},
    /*r26*/ {5,5,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,5,5,5,5,5,5,5,5},
    /*r27*/ {5,5,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,5,5,5,5,5,5,5,5},
    /*r28*/ {5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5},
    /*r29*/ {5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5}
};

static void map_load(void) {
    volatile u16 *sbb28 = BG_SCREENBLOCK(28);
    volatile u16 *sbb29 = BG_SCREENBLOCK(29);
    for (int row = 0; row < 30; row++) {
        for (int col = 0; col < 40; col++) {
            u16 entry = farm_map[row][col];
            if (col < 32)
                sbb28[row * 32 + col] = entry;
            else
                sbb29[row * 32 + (col - 32)] = entry;
        }
    }
}

void map_init(void) {
    for (int i = 0; i < 13; i++)
        BG_PALETTE[i] = tile_colors[i];

    for (int t = 1; t <= 12; t++) {
        volatile u32 *tile = BG_CHARBLOCK(0) + t * 8;
        u32 word = (u32)t * 0x11111111u;
        for (int r = 0; r < 8; r++) tile[r] = word;
    }
    map_load();
}

u8 map_tile_at(int wx, int wy) {
    int col = wx / 8;
    int row = wy / 8;
    if (col < 0 || col >= 40 || row < 0 || row >= 30) return TILE_TREE;
    if (farm_state[row][col] != 0) return farm_state[row][col];
    return farm_map[row][col];
}

int map_is_solid(u8 tile_id) {
    return tile_id == TILE_WATER ||
           tile_id == TILE_TREE ||
           tile_id == TILE_FENCE ||
           tile_id == TILE_WALL;
}

void map_till(int wx, int wy) {
    int col = wx / 8;
    int row = wy / 8;

    if (col < 0 || col >= 40 || row < 0 || row >= 30) return;
    if (farm_map[row][col] != TILE_DIRT) return;

    farm_state[row][col] = TILE_TILLED;
    volatile u16 *sbb = (col < 32) ? BG_SCREENBLOCK(28) : BG_SCREENBLOCK(29);
    int scol = (col < 32) ? col : col - 32;
    sbb[row * 32 + scol] = TILE_TILLED;
}

void map_water(int wx, int wy) {
    int col = wx / 8;
    int row = wy / 8;

    if (col < 0 || col >= 40 || row < 0 || row >= 30) return;
    if (farm_state[row][col] != TILE_TILLED) return;

    farm_state[row][col] = TILE_WATERED;

    volatile u16 *sbb = (col < 32) ? BG_SCREENBLOCK(28) : BG_SCREENBLOCK(29);
    int scol = (col < 32) ? col : col - 32;
    sbb[row * 32 + scol] = TILE_WATERED;
}

void map_dry_fields(void) {
    for (int row = 0; row < 30; row++) {
        for (int col = 0; col < 40; col++) {
            if (farm_state[row][col] != TILE_WATERED) continue;
            farm_state[row][col] = TILE_TILLED;
            volatile u16 *sbb = (col < 32) ? BG_SCREENBLOCK(28) : BG_SCREENBLOCK(29);
            int scol = (col < 32) ? col : col - 32;
            sbb[row * 32 + scol] = TILE_TILLED;
        }
    }
}

void map_plant (int wx, int wy) {
    int col = wx / 8;
    int row = wy / 8;
    if (col < 0 || col >= 40 || row < 0 || row >= 30) return;
    if (farm_state[row][col] != TILE_WATERED) return;
    farm_state[row][col] = TILE_PLANTED;
    crop_age[row][col] = 0;
    volatile u16 *sbb = (col < 32) ? BG_SCREENBLOCK(28) : BG_SCREENBLOCK(29);
    int scol = (col < 32) ? col : col - 32;
    sbb[row * 32 + scol] = TILE_PLANTED;
}

void map_grow_crops (void) {
    for (int row = 0; row < 30; row++) {
        for (int col = 0; col < 40; col++) {
            if (farm_state[row][col] != TILE_PLANTED) continue;
            crop_age[row][col]++;
            if (crop_age[row][col] < 4) continue;
            farm_state[row][col] = TILE_GROWN;
            volatile u16 *sbb = (col < 32) ? BG_SCREENBLOCK(28) : BG_SCREENBLOCK(29);
            int scol = (col < 32) ? col : col - 32;
            sbb[row * 32 + scol] = TILE_GROWN;
        }
    }
}

void map_harvest(int wx, int wy) {
    int col = wx / 8;
    int row = wy / 8;
    if (col < 0 || col >= 40 || row < 0 || row >= 30) return;
    if (farm_state[row][col] != TILE_GROWN) return;
    farm_state[row][col] = TILE_TILLED;
    crop_age[row][col] = 0;
    volatile u16 *sbb = (col < 32) ? BG_SCREENBLOCK(28) : BG_SCREENBLOCK (29);
    int scol = (col < 32) ? col : col - 32;
    sbb[row * 32 + scol] = TILE_TILLED;
}

void map_wilt_crops (void) {
    for (int row = 0; row < 30; row++) {
        for (int col = 0; col < 40; col++) {
            u8 s = farm_state[row][col];
            if (s!= TILE_PLANTED && s != TILE_GROWN) continue;
            farm_state[row][col] = TILE_TILLED;
            crop_age[row][col] = 0;
            volatile u16 *sbb = (col < 32) ? BG_SCREENBLOCK(28) : BG_SCREENBLOCK(29);
            int scol = (col < 32) ? col : col - 32;
            sbb[row * 32 + scol] = TILE_TILLED;
        }
    }
}

int map_is_adjacent_water(int wx, int wy) {
    return  map_tile_at(wx,      wy - 8)    == TILE_WATER ||
            map_tile_at(wx,      wy + 8)    == TILE_WATER ||
            map_tile_at(wx - 8,  wy)        == TILE_WATER ||
            map_tile_at(wx + 8,  wy)        == TILE_WATER;
}