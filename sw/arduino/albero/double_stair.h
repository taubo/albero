#ifndef DOUBLE_STAIR_H
#define DOUBLE_STAIR_H

#include "color_seq.h"

struct double_stairs_config {
	uint8_t max_value;
	uint8_t red_default;
	uint8_t green_default;
	uint8_t blue_default;
};

void double_stair_init(void);
void double_stair_update(struct color_seq *col, void *data);

#endif
