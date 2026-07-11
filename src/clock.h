#pragma once
#include "gba.h"

typedef struct {
    int tick;
    int minute;
    int day;
    int season;
} Clock;

void clock_init(Clock *c);
void clock_tick(Clock *c);
int clock_hour(const Clock *c);
int clock_min(const Clock *c);