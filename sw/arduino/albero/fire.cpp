#include "fire.h"
#include <Arduino.h>

int r = 120; 
int g = r - 70;
int b = 10;

void fire_update(struct color_seq *col, void *data)
{
	int i;

	for (i = 0; i < col->len; ++i) {
		int flicker = random(0, 25);
		int r1 = r - flicker;
		int g1 = g - flicker;
		int b1 = b - flicker;
		if (r1 < 0)
			r1 = 0;
		if (g1 < 0)
			g1 = 0;
		if (b1 < 0)
			b1 = 0;
		col->colors[i] = rgb_color(g1, r1, b1);
	}
}
