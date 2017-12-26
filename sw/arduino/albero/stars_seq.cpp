#include <stdint.h>
#include <string.h>
#include "stars_seq.h"

// seq stars
#define LED_NUMBER        100
#define MAX_STARS         20
#define MAX_BRIGHT        50
#define MIN_BRIGHT        10
#define BRIGHT_INCREMENT  2
#define BRIGHT_DECREMENT  1

int current_stars;
struct stars_seq_params stars_params;
uint8_t led_free[LED_NUMBER];
struct star_seq global_seq[LED_NUMBER];

/*
   update(input, output, state)
 */
void update(struct star_seq *seq) {
	uint8_t bright;
	uint8_t bright_increment;
	uint8_t bright_decrement;

	if (!seq)
		return;

	bright_increment = seq->bright_increment;
	bright_decrement = seq->bright_decrement;
	bright = seq->bright;

	switch (seq->state) {
		case STAR_START:
			bright = MIN_BRIGHT;
			seq->state = STAR_ASC;
			break;
		case STAR_ASC:
			if (bright + bright_increment >= MAX_BRIGHT) {
				bright = MAX_BRIGHT;
				seq->state = STAR_DESC;
			} else {
				bright += bright_increment;
			}
			break;
		case STAR_DESC:
			if (bright - bright_decrement <= MIN_BRIGHT) {
				bright = MIN_BRIGHT;
				seq->state = STAR_END;
			} else {
				bright -= bright_decrement;
			}
			break;
		case STAR_END:
			bright = 0;
			seq->state = STAR_START;
			break;
	}

	seq->bright = bright;
}

rgb_color default_animated(uint8_t bright)
{
	return rgb_color(bright, 0, bright / 10);
}

void stars_init()
{
	current_stars = 0;

	stars_params.max_stars = MAX_STARS;
	stars_params.bright_increment = BRIGHT_INCREMENT;
	stars_params.bright_decrement = BRIGHT_DECREMENT;
	stars_params.fixed_led_color = {
		.red = MIN_BRIGHT,
		.green = 0,
		.blue = MIN_BRIGHT / 10,
	};
	stars_params.animated_led_color = default_animated;

	for (int i = 0; i < LED_NUMBER; ++i) {
		global_seq[i].bright = 0;
		global_seq[i].bright_increment = BRIGHT_INCREMENT;
		global_seq[i].bright_decrement = BRIGHT_DECREMENT;
		global_seq[i].state = STAR_START;
		led_free[i] = 1;
		// pinMode(led_pins[i], OUTPUT);
	}
}

// void stars_update()
void stars_update(struct color_seq *col, void *data)
{
	int next_star = -1;

	if (current_stars < MAX_STARS) {
		next_star = rand() % LED_NUMBER;
	}

	for (int i = 0; i < LED_NUMBER; ++i) {
		if ((i == next_star) && (led_free[i])) {
			led_free[i] = 0;
			current_stars++;
			update(&global_seq[i]);
		} else if (!led_free[i]) {
			if (global_seq[i].state == STAR_END) {
				current_stars--;
				led_free[i] = 1;       
			}
			update(&global_seq[i]);
		}

		if (led_free[i])
			// col->colors[i] = rgb_color(0, 0, 0);
			col->colors[i] = stars_params.fixed_led_color;
			// col->colors[i] = rgb_color(0, 20, 0);
		else
			// col->colors[i] = rgb_color(global_seq[i].bright, global_seq[i].bright, global_seq[i].bright);
			// col->colors[i] = rgb_color(0, global_seq[i].bright, global_seq[i].bright);
			// col->colors[i] = rgb_color(global_seq[i].bright, 0, global_seq[i].bright);
			// col->colors[i] = rgb_color(global_seq[i].bright, global_seq[i].bright, 0);
			// col->colors[i] = rgb_color(0, global_seq[i].bright, 0);
			col->colors[i] = stars_params.animated_led_color(global_seq[i].bright);
	}
}

void stars_get_default_params(struct stars_seq_params *params)
{
	if (!params)
		return;

	memcpy(params, &stars_params, sizeof(struct stars_seq_params));
}

int stars_seq_set_params(const struct stars_seq_params *params)
{
	int i;

	if (!params)
		return -1;

	memcpy(&stars_params, params, sizeof(struct stars_seq_params));
	for (i = 0; i < LED_NUMBER; ++i) {
		global_seq[i].bright_increment = stars_params.bright_increment;
		global_seq[i].bright_decrement = stars_params.bright_decrement;
	}

	return 0;
}
