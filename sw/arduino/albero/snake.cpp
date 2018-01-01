#include "snake.h"
#include <string.h>

#define TEST

int last_led_idx;
rgb_color main_color;
rgb_color bg_color;

struct snake_config cfg;

void snake_set_config(struct snake_config config)
{
	cfg = config;
}

void snake_set_color(rgb_color color)
{
	main_color = color;
}

void snake_set_bg_color(rgb_color color)
{
	bg_color = color;
}

void snake_init(void)
{
	last_led_idx = -1;
	main_color = rgb_color(0, 0, 0);
	memset(&cfg, 0, sizeof(struct snake_config));
}

void snake_update(struct color_seq *col, void *data)
{
	if (last_led_idx < 0)
		for (int i = 0; i < col->len; ++i)
			col->colors[i] = bg_color;

	if (last_led_idx < col->len) {
		col->colors[last_led_idx++] = main_color;
	} else {
#ifdef TEST
		if (cfg.reset_colors)
			bg_color = rgb_color(0, 0, 0);
		else
			bg_color = main_color;
		main_color = rgb_color(rand() % 50, rand() % 50, rand() % 50);
#endif
		last_led_idx = -1;
	}
}
