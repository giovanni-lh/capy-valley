#pragma once
#include "clock.h"

void hud_init(void);
void hud_update(const Clock *c);
void hud_set_coins(int n);