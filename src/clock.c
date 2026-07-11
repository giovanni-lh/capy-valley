#include "clock.h"

void clock_init(Clock *c) {
    c->tick = 0;
    c->minute = 0;
    c->day = 0;
    c->season = 0;
}

void clock_tick(Clock *c) {
    c->tick++;
    if (c->tick < 60) return;
    c->tick = 0;
    c->minute++;
    if (c->minute < 1440) return;
    c->minute = 0;
    c->day++;
    if (c->day < 28) return;
    c->day = 1;
    c->season = (c->season + 1) & 3;
}

int clock_hour(const Clock *c) {
    return (c->minute / 60 + 6) % 24;
}
int clock_min(const Clock *c) {
    return c->minute % 60;
}