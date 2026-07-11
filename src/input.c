#include "input.h"

static u16 s_curr = 0;

void input_update(void) {
    s_curr = ~(REG_KEYINPUT) & 0x03FFu;
}

int key_held(u16 key) {
    return (s_curr & key) != 0;
}