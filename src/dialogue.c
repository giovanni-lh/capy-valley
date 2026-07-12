#include "dialogue.h"
#include "gba.h"

/* Pack 8 nibbles into one u32 tile row. a = leftmost pixel. */
#define R8(a,b,c,d,e,f,g,h) \
    ((u32)(a) | ((u32)(b) << 4) | ((u32)(c) << 8) | ((u32)(d) << 12) | \
     ((u32)(e) << 16) | ((u32)(f) << 20) | ((u32)(g) << 24) | ((u32)(h) << 28))

/* Select palette bank 2 for a screenblock entry. */
#define DLG_TILE(n) ((u16)((n) | (2u << 12)))

/* Pixel values: 1 = dark background (palette bank 2, index 1)
   2 = text color (palette bank 2, index 2) */
#define B 1
#define T 2

/* font_tiles[0] = space, font_tiles[1..26] = A..Z.
   Each glyph is 8 rows of 8 pixels, stored as u32 words.
   Pixels are either B (dark background) or T (text color). */
static const u32 font_tiles[27][8] = {
    /* space */
    { R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* A */
    { R8(B,T,T,T,B,B,B,B), R8(T,B,B,B,T,B,B,B),
      R8(T,B,B,T,B,B,B,B), R8(T,T,T,T,T,B,B,B),
      R8(T,B,B,B,T,B,B,B), R8(T,B,B,B,T,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* B */
    { R8(T,T,T,B,B,B,B,B), R8(T,B,B,T,B,B,B,B),
      R8(T,T,T,B,B,B,B,B), R8(T,B,B,T,B,B,B,B),
      R8(T,T,T,B,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* C */
    { R8(B,T,T,T,B,B,B,B), R8(T,B,B,B,T,B,B,B),
      R8(T,B,B,B,B,B,B,B), R8(T,B,B,B,T,B,B,B),
      R8(B,T,T,T,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* D */
    { R8(T,T,T,B,B,B,B,B), R8(T,B,B,T,B,B,B,B),
      R8(T,B,B,T,B,B,B,B), R8(T,B,B,T,B,B,B,B),
      R8(T,T,T,B,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* E */
    { R8(T,T,T,T,B,B,B,B), R8(T,B,B,B,B,B,B,B),
      R8(T,T,T,B,B,B,B,B), R8(T,B,B,B,B,B,B,B),
      R8(T,T,T,T,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* F */
    { R8(T,T,T,T,B,B,B,B), R8(T,B,B,B,B,B,B,B),
      R8(T,T,T,B,B,B,B,B), R8(T,B,B,B,B,B,B,B),
      R8(T,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* G */
    { R8(B,T,T,T,B,B,B,B), R8(T,B,B,B,B,B,B,B),
      R8(T,B,T,T,B,B,B,B), R8(T,B,B,T,B,B,B,B),
      R8(B,T,T,T,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* H */
    { R8(T,B,B,T,B,B,B,B), R8(T,B,B,T,B,B,B,B),
      R8(T,T,T,T,B,B,B,B), R8(T,B,B,T,B,B,B,B),
      R8(T,B,B,T,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* I */
    { R8(T,T,T,B,B,B,B,B), R8(B,T,B,B,B,B,B,B),
      R8(B,T,B,B,B,B,B,B), R8(B,T,B,B,B,B,B,B),
      R8(T,T,T,B,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* J */
    { R8(B,B,T,T,B,B,B,B), R8(B,B,B,T,B,B,B,B),
      R8(B,B,B,T,B,B,B,B), R8(T,B,B,T,B,B,B,B),
      R8(B,T,T,B,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* K */
    { R8(T,B,T,B,B,B,B,B), R8(T,B,T,B,B,B,B,B),
      R8(T,T,B,B,B,B,B,B), R8(T,B,T,B,B,B,B,B),
      R8(T,B,T,B,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* L */
    { R8(T,B,B,B,B,B,B,B), R8(T,B,B,B,B,B,B,B),
      R8(T,B,B,B,B,B,B,B), R8(T,B,B,B,B,B,B,B),
      R8(T,T,T,T,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* M */
    { R8(T,B,B,T,B,B,B,B), R8(T,T,B,T,B,B,B,B),
      R8(T,B,B,T,B,B,B,B), R8(T,B,B,T,B,B,B,B),
      R8(T,B,B,T,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* N */
    { R8(T,B,B,T,B,B,B,B), R8(T,T,B,T,B,B,B,B),
      R8(T,B,T,T,B,B,B,B), R8(T,B,B,T,B,B,B,B),
      R8(T,B,B,T,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* O */
    { R8(B,T,T,T,B,B,B,B), R8(T,B,B,B,T,B,B,B),
      R8(T,B,B,B,T,B,B,B), R8(T,B,B,B,T,B,B,B),
      R8(B,T,T,T,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* P */
    { R8(T,T,T,B,B,B,B,B), R8(T,B,B,T,B,B,B,B),
      R8(T,T,T,B,B,B,B,B), R8(T,B,B,B,B,B,B,B),
      R8(T,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* Q */
    { R8(B,T,T,T,B,B,B,B), R8(T,B,B,B,T,B,B,B),
      R8(T,B,B,T,T,B,B,B), R8(T,B,T,T,T,B,B,B),
      R8(B,B,B,T,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* R */
    { R8(T,T,T,B,B,B,B,B), R8(T,B,B,T,B,B,B,B),
      R8(T,T,T,B,B,B,B,B), R8(T,B,B,T,B,B,B,B),
      R8(T,B,B,T,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* S */
    { R8(B,T,T,T,B,B,B,B), R8(T,B,B,B,T,B,B,B),
      R8(B,T,B,T,B,B,B,B), R8(B,B,B,T,B,B,B,B),
      R8(T,T,T,B,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* T */
    { R8(T,T,T,T,T,B,B,B), R8(B,B,T,B,B,B,B,B),
      R8(B,B,T,B,B,B,B,B), R8(B,B,T,B,B,B,B,B),
      R8(B,B,T,B,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* U */
    { R8(T,B,B,B,T,B,B,B), R8(T,B,B,B,T,B,B,B),
      R8(T,B,B,B,T,B,B,B), R8(B,T,T,T,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* V */
    { R8(T,B,B,B,T,B,B,B), R8(T,B,B,B,T,B,B,B),
      R8(T,B,B,B,T,B,B,B), R8(B,T,B,T,B,B,B,B),
      R8(B,B,T,B,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* W */
    { R8(T,B,B,B,T,B,B,B), R8(T,B,T,B,T,B,B,B),
      R8(T,B,T,B,T,B,B,B), R8(T,B,T,B,T,B,B,B),
      R8(B,T,B,T,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* X */
    { R8(T,B,B,T,B,B,B,B), R8(B,T,B,T,B,B,B,B),
      R8(B,B,T,B,B,B,B,B), R8(B,T,B,T,B,B,B,B),
      R8(T,B,B,T,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* Y */
    { R8(T,B,B,T,B,B,B,B), R8(B,T,B,T,B,B,B,B),
      R8(B,T,B,B,B,B,B,B), R8(B,T,B,B,B,B,B,B),
      R8(B,T,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) },
    /* Z */
    { R8(T,T,T,T,T,B,B,B), R8(B,B,B,T,B,B,B,B),
      R8(B,B,T,B,B,B,B,B), R8(T,T,T,T,T,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B),
      R8(B,B,B,B,B,B,B,B), R8(B,B,B,B,B,B,B,B) }
};

#undef B
#undef T

void dlg_init(void) {
    BG_PALETTE[32] = RGB15(0, 0, 0);       /* bank 2 index 0: transparent */
    BG_PALETTE[33] = RGB15(4, 3, 2);       /* bank 2 index 1: dark box */
    BG_PALETTE[34] = RGB15(28, 28, 20);    /* bank 2 index 2: off-white text */

    volatile u32 *cbb2 = BG_CHARBLOCK(2);

    /* Tile 1: solid dark (used for box border and background) */
    for (int r = 0; r < 8; r++) cbb2[1 * 8 + r] = 0x11111111u;

    /* Tiles 2-28: space (index 0) and A-Z (indices 1-26) */
    for (int g = 0; g < 27; g++) {
        volatile u32 *dst = cbb2 + (g + 2) * 8;
        for (int r = 0; r < 8; r++) dst[r] = font_tiles[g][r];
    }

    /* Clear SBB31 */
    volatile u16 *sbb = BG_SCREENBLOCK(31);
    for (int i = 0; i < 32 * 32; i++) sbb[i] = 0;
}

void dlg_show(const char *text) {
    volatile u16 *sbb = BG_SCREENBLOCK(31);

    /* Fill 3-row box with solid dark tile */
    for (int col = 0; col < 30; col++) {
        sbb[17 * 32 + col] = DLG_TILE(1);
        sbb[18 * 32 + col] = DLG_TILE(1);
        sbb[19 * 32 + col] = DLG_TILE(1);
    }

    /* Write text on row 18, columns 1-28 */
    int col = 1;
    for (int i = 0; text[i] != '\0' && col < 29; i++, col++) {
        char c = text[i];
        int idx;
        if (c == ' ')
            idx = 0;
        else if (c >= 'A' && c <= 'Z')
            idx = 3 + (c - 'A'); // Adjust index based on font_tiles array
        else
            idx = 2;
        sbb[18 * 32 + col] = DLG_TILE(idx);
    }
}

void dlg_hide(void) {
    volatile u16 *sbb = BG_SCREENBLOCK(31);
    for (int col = 0; col < 30; col++) {
        sbb[17 * 32 + col] = 0;
        sbb[18 * 32 + col] = 0;
        sbb[19 * 32 + col] = 0;
    }
}