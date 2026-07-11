#pragma once
#include <stdint.h>

/* — Common typedefs ———————————————————————————————————————— */
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int16_t s16;
typedef int32_t s32;

/* — Screen —————————————————————————————————————————————————— */
#define SCREEN_W 240
#define SCREEN_H 160

/* — Color helper ———————————————————————————————————————————— */
/* Each channel 0-31. GBA uses 15-bit BGR color stored in 16 bits. */
#define RGB15(r, g, b) ((u16)(((b) << 10) | ((g) << 5) | (r)))

/* — Display control ————————————————————————————————————————— */
#define REG_DISPCNT (*(volatile u16*)0x04000000)
#define REG_VCOUNT  (*(volatile u16*)0x04000006)

#define DCNT_MODE0  0x0000        /* tiled BG mode */
#define DCNT_OBJ_1D (1 << 6)      /* OBJ tiles laid out linearly in memory */
#define DCNT_BG0    (1 << 8)
#define DCNT_BG1    (1 << 9)
#define DCNT_BG2    (1 << 10)
#define DCNT_BG3    (1 << 11)
#define DCNT_OBJ    (1 << 12)

/* — BG Layer 2 registers (we use BG2 for the terrain Layer) —————————— */
#define REG_BG0CNT  (*(volatile u16*)0x04000008)
#define REG_BG2CNT  (*(volatile u16*)0x0400000C)
#define REG_BG2HOFS (*(volatile u16*)0x04000018) /* write-only */
#define REG_BG2VOFS (*(volatile u16*)0x0400001A) /* write-only */

/* BGxCNT field helpers */
#define BG_PRIO(n)    ((n) & 0x3)                 /* draw priority 0 (front) - 3 (back) */
#define BG_CBB(n)     (((n) & 0x3) << 2)          /* charblock base (tile pixel data) */
#define BG_SBB(n)     (((n) & 0x1F) << 8)         /* screenblock base (tile map) */
#define BG_4BPP       0x0000                      /* 16 colors per tile */
#define BG_SIZE_32x32 (0 << 14)                   /* 256x256 pixels */
#define BG_SIZE_64x32 (1 << 14)                   /* 512x256 pixels - 2 screenblocks wide */
#define BG_SIZE_32x64 (2 << 14)
#define BG_SIZE_64x64 (3 << 14)

/* — Palette RAM ————————————————————————————————————————————— */
#define BG_PALETTE  ((volatile u16*)0x05000000)   /* 256 BG colors */
#define OBJ_PALETTE ((volatile u16*)0x05000200)   /* 256 OBJ colors */

/* — VRAM pointers ——————————————————————————————————————————— */
/* Charblock n: holds tile pixel data. Each block = 16 KB = 512 4bpp tiles.
   BG tiles live in charblocks 0-3 (0x06000000 - 0x0600FFFF).
   OBJ tiles live in charblocks 4-5 (0x06010000 - 0x06017FFF). */
#define BG_CHARBLOCK(n) ((volatile u32*)(0x06000000 + (n) * 0x4000))

/* Screenblock n: holds the tile map (which tile to draw at each grid cell).
   Each block = 2 KB = 32x32 u16 entries.
   32 screenblocks fit inside the BG VRAM region. */
#define BG_SCREENBLOCK(n) ((volatile u16*)(0x06000000 + (n) * 0x800))

/* OBJ (sprite) tile area - separate from BG tiles in Mode 0. */
#define OBJ_TILES ((volatile u32*)0x06010000)

/* — OAM (sprite attributes) ————————————————————————————————— */
/* Each OAM entry = 8 bytes: three 16-bit attribute words + 2-byte padding. */
typedef struct {
    u16 attr0;
    u16 attr1;
    u16 attr2;
    u16 pad;    /* unused in non-affine mode; do not touch */
} OamEntry;

#define OAM ((volatile OamEntry*)0x07000000) /* 128 entries */

/* attr0 - Y position and sprite mode */
#define ATTR0_Y(n)      ((n) & 0xFF)      /* bits 0-7: Y screen position */
#define ATTR0_NORMAL    (0 << 8)          /* bits 8-9 = 00: standard sprite */
#define ATTR0_HIDDEN    (2 << 8)          /* bits 8-9 = 10: sprite disabled */
#define ATTR0_CLR4      (0 << 12)         /* bit 12 = 0: 4bpp (16-color palette) */
#define ATTR0_SQUARE    (0 << 14)         /* bits 14-15 = 00: square shape */

/* attr1 - X position and sprite size */
#define ATTR1_X(n)      ((n) & 0x1FF)     /* bits 0-8: X screen position (9-bit) */
#define ATTR1_SZ16      (1 << 14)         /* bits 14-15 = 01: 16x16 when shape=SQUARE */

/* attr2 - tile index, priority, palette bank */
#define ATTR2_TILE(n)    ((n) & 0x3FF)    /* bits 0-9: OBJ tile index */
#define ATTR2_PRIO(n)    ((n & 3) << 10)
#define ATTR2_PALETTE(n) ((n & 0xF) << 12)

/* — Keys ———————————————————————————————————————————————————— */
/* REG_KEYINPUT is active-low: bit = 0 when the button is pressed.
   The input module inverts this so 1 = pressed everywhere else. */
#define REG_KEYINPUT (*(volatile u16*)0x04000130)

#define KEY_A      (1u << 0)
#define KEY_B      (1u << 1)
#define KEY_SELECT (1u << 2)
#define KEY_START  (1u << 3)
#define KEY_RIGHT  (1u << 4)
#define KEY_LEFT   (1u << 5)
#define KEY_UP     (1u << 6)
#define KEY_DOWN   (1u << 7)
#define KEY_R      (1u << 8)
#define KEY_L      (1u << 9)

/* — VBlank sync ————————————————————————————————————————————— */
/* Spin-wait until the start of VBlank (scanline 160).
   Call once at the top of each game loop iteration.
   All OAM and palette writes should happen after this returns. */
static inline void vsync(void) {
    while (REG_VCOUNT >= 160) {} /* wait out current VBlank if we're in one */
    while (REG_VCOUNT < 160)  {} /* wait for VBlank to begin */
}