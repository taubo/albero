#include "func_seq.h"
#include <Arduino.h>
#include <math.h>
#include <string.h>

#define PI	3.14159265359
double pi = 3.14159265359;

struct rgb local_col = {
	.red = 20,
	.green = 0,
	.blue = 0,
};

void func_seq_rand_color(struct rgb col)
{
	memcpy(&local_col, &col, sizeof(struct rgb));
}

void func_seq_update(struct color_seq *col, void *data)
{
	int i;
	double val;
	int r, g, b;

	for (i = 0; i < col->len; ++i) {
		// val = i * PI / 10000 + millis() / 10;
		// val = (i + millis() / 50) * PI / 8;
		val = (i + millis() / 40) * PI / 32;
		r = (sin(val) * local_col.red) + local_col.red;
		g = (sin(val) * local_col.green) + local_col.green;
		b = (sin(val) * local_col.blue) + local_col.blue;
		col->colors[i] = rgb_color(g, r, b);
	}
}
