#pragma once
#include <pebble.h>

#define HEALTH_STEPS_GOAL 10000


void health_init();

int health_get_steps_today();
