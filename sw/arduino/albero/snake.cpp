#include "snake.h"

#define TEST

int last_led_idx;
rgb_color main_color;

void snake_set_color(rgb_color color)
{
	main_color = color;
}

void snake_init(void)
{
	last_led_idx = -1;
	main_color = rgb_color(0, 0, 0);
}

void snake_update(struct color_seq *col, void *data)
{
	if (last_led_idx < 0)
		for (int i = 0; i < col->len; ++i)
			col->colors[i] = rgb_color(0, 0, 0);

	if (last_led_idx < col->len) {
		col->colors[last_led_idx++] = main_color;
	} else {
#ifdef TEST
		main_color = rgb_color(rand() % 50, rand() % 50, rand() % 50);
#endif
		last_led_idx = -1;
	}
}
