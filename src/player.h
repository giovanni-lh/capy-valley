#pragma once
#include "gba.h"

typedef struct {
    int wx;
    int wy;
} Player;

void player_init(Player *p, int start_wx, int start_wy);
void player_update(Player *p);