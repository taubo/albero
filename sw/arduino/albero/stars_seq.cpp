#include <stdint.h>
#include "stars_seq.h"

// seq stars
#define LED_NUMBER        100
#define MAX_STARS         20
#define MAX_BRIGHT        50
#define BRIGHT_INCREMENT  2
#define BRIGHT_DECREMENT  1

int current_stars;

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
			bright = 0;
			seq->state = STAR_ASC;
			break;
		case STAR_ASC:
			if (bright + bright_increment >= MAX_BRIGHT) {
				Serial.println("next desc");
				bright = MAX_BRIGHT;
				seq->state = STAR_DESC;
			} else {
				bright += bright_increment;
			}
			break;
		case STAR_DESC:
			if (bright - bright_decrement <= 0) {
				bright = 0;
				seq->state = STAR_END;
			} else {
				bright -= bright_decrement;
			}
			break;
		case STAR_END:
			Serial.println("end");
			bright = 0;
			seq->state = STAR_START;
			break;
	}

	seq->bright = bright;
}

void stars_init()
{
	current_stars = 0;

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
			col->colors[i] = rgb_color(0, 0, 0);
		// colors[i] = rgb_color(global_seq[i].bright, 0, global_seq[i].bright);
		else
			//colors[i] = rgb_color(0, 0, 0);
			col->colors[i] = rgb_color(global_seq[i].bright, global_seq[i].bright, global_seq[i].bright);
		// analogWrite(led_pins[i], global_seq[i].bright);
	}
}
