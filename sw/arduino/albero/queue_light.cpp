#include "queue_light.h"

static int state;

struct rgb def_color = {
	.red = 20,
	.green = 120,
	.blue = 80,
};

struct rgb col_seq;

void queue_light_set_color(struct rgb color)
{
	col_seq = color;
}

void queue_light_init()
{
	col_seq = def_color;
}

void queue_light_update(struct color_seq *col, void *data)
{
	int i;

	if (state == 0) {
		for (i = 0; i < col->len; ++i) {
			if (i % 2) {
				col->colors[i] = rgb_color(0, 0, 0);
			} else {
				col->colors[i] = rgb_color(col_seq.green, col_seq.red, col_seq.blue);
			}
		}
		state = 1;
	} else if (state == 1) {
		for (i = 0; i < col->len; ++i) {
			if (i % 2) {
				col->colors[i] = rgb_color(col_seq.green, col_seq.red, col_seq.blue);
			} else {
				col->colors[i] = rgb_color(0, 0, 0);
			}
		}
		state = 0;
	}
}
