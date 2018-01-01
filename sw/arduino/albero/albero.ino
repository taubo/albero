#include <string.h>
#include <stdlib.h>

#include <PololuLedStrip.h>

#include "rainbow_seq.h"
#include "stars_seq.h"
#include "double_stair.h"
#include "snake.h"
#include "func_seq.h"
#include "fire.h"
#include "queue_light.h"

// Create an ledStrip object and specify the pin it will use.
PololuLedStrip<2> ledStrip;

#define BUTTON_STATE_PRESSED	0
#define BUTTON_STATE_RELEASED	1

#define BUTTON_NUMBER		2
#define BUTTON1_GPIO_PIN	0
#define BUTTON2_GPIO_PIN	1

#define LONG_PRESS_DURATION	3000

#define EVT_BTN_NO_EVT		0
#define EVT_BTN_PRESS_SHORT	1
#define EVT_BTN_PRESS_LONG	2

#define SEQ_COUNT		6

/*
 * system
 */
unsigned long get_system_time(void)
{
	return millis();
}

/*
 * buttons
 */
struct button_press {
	int state;		// BUTTON_STATE_PRESSED, BUTTON_STATE_RELEASED
	unsigned long start_time;
	// unsigned long duration;
	int duration;
};

struct button_evt {
	int press_event;	// long or short press
};

struct buttons {
	int gpio;
	struct button_press btn_press;
	struct button_evt btn_evt;
};

void button_press_update(int value, struct button_press *button)
{
	switch (button->state) {
	case BUTTON_STATE_RELEASED:
		if (value == 1) {
			button->start_time = get_system_time();
			button->state = BUTTON_STATE_PRESSED;
		}
		break;
	case BUTTON_STATE_PRESSED:
		if (value == 0) {
			button->state = BUTTON_STATE_RELEASED;
		}
		button->duration = get_system_time() - button->start_time;
		break;
	default:
		break;
	}
}

/*
 * task
 */
struct albetask {
	void (* execute)(void *data);
};

/*
 * main program
 */

struct buttons alb_buttons[BUTTON_NUMBER];
struct albetask button_task;

void button_execute(void *data)
{
	int i;

	for (i = 0; i < BUTTON_NUMBER; ++i) {
		button_press_update(digitalRead(alb_buttons[i].gpio), &alb_buttons[i].btn_press);

		if (alb_buttons[i].btn_press.duration >= LONG_PRESS_DURATION) {
			alb_buttons[i].btn_evt.press_event = EVT_BTN_PRESS_LONG;
		} else {
			if (alb_buttons[i].btn_press.state == BUTTON_STATE_RELEASED) {
				if (alb_buttons[i].btn_press.duration > 10)
					alb_buttons[i].btn_evt.press_event = EVT_BTN_PRESS_SHORT;
			}
		}
	}
}

#define ALBERO_STATE_NORMAL	0
#define ALBERO_CYCLIC_SEQ_STATE	1

struct color_seq tree_colors;
struct light_seq light_sequence[SEQ_COUNT];
int seq_idx;
int push_counter;
unsigned int seq_dur, seq_start;
bool new_state;

int albero_state;

/*
 * every color is in the form of c * bright / lum
 * where:
 *	- c = {0, 1};
 * 	- 0 <= lum <= bright
 */
struct stars_color_params {
	uint8_t c[3];
	uint8_t lum[3];
};

struct stars_color_params gen_col;

void generate_stars_color(struct stars_color_params *col_params)
{
	int i;
	int total_c;

	for (i = 0; i < 3; ++i) {
		col_params->c[i] = rand() % 2;
		total_c += col_params->c[i];
		col_params->lum[i] = rand() % 256;
	}

	if (total_c == 0)
		col_params->c[rand() % 3] = 1;
}

rgb_color albero_animated_stars(uint8_t bright)
{
	return rgb_color(gen_col.c[0] * bright, gen_col.c[1] * bright, gen_col.c[2] * bright);
}

void set_random_stars()
{
	int rng_conf;

	rng_conf = rand() % 2;

	struct stars_seq_params local_params;
	stars_get_default_params(&local_params);
	local_params.animated_led_color = albero_animated_stars;

	generate_stars_color(&gen_col);
	switch (rng_conf) {
	case 0:
		local_params.fixed_led_color = rgb_color(0, 0, 0);
		break;
	case 1:
		uint8_t bright = rand() % 256;
		local_params.fixed_led_color = rgb_color(gen_col.c[0] * bright / 25, gen_col.c[1] * bright / 25, gen_col.c[2] * bright / 25);
		break;
	}
	stars_seq_set_params(&local_params);
}

void setup() {
	Serial.begin(9600);
	pinMode(0, INPUT);
	pinMode(1, INPUT);

	// init_button_press
	memset(alb_buttons, 0, sizeof(struct buttons) * BUTTON_NUMBER);
	alb_buttons[0].gpio = BUTTON1_GPIO_PIN;
	alb_buttons[1].gpio = BUTTON2_GPIO_PIN;
	alb_buttons[0].btn_press.duration = 0UL;
	alb_buttons[1].btn_press.duration = 0UL;

	// init button task
	button_task.execute = button_execute;

	push_counter = 0;
	albero_state = ALBERO_STATE_NORMAL;

	// color seq
	seq_idx = 0;
	tree_colors.len = 100;
	tree_colors.colors = (rgb_color *)malloc(100 * sizeof (rgb_color));

	light_sequence[0].update = rainbow_update;
	light_sequence[0].period = 50;

	// light_seq[1].init = stars_init;
	stars_init();
	light_sequence[1].update = stars_update;
	light_sequence[1].period = 50;

	double_stair_init();
	light_sequence[2].update = double_stair_update;
	light_sequence[2].period = 50;

	snake_init();
	light_sequence[3].update = snake_update;
	light_sequence[3].period = 50;

	light_sequence[4].update = func_seq_update;
	light_sequence[4].period = 50;

	light_sequence[5].update = fire_update;
	light_sequence[5].period = 50;

/*
	queue_light_init();
	light_sequence[6].update = queue_light_update;
	light_sequence[6].period = 500;
	*/
}

void loop() {
	if (button_task.execute)
		button_task.execute(NULL);

	/*
	char press_type[10];
	for (int i = 0; i < BUTTON_NUMBER; ++i) {
		if (alb_buttons[i].btn_evt.press_event == EVT_BTN_PRESS_SHORT)
			strcpy(press_type, "SHORT");
		else if (alb_buttons[i].btn_evt.press_event == EVT_BTN_PRESS_LONG)
			strcpy(press_type, "LONG");
		else
			strcpy(press_type, "NONE");
		Serial.print("button ");
		Serial.print(i);
		Serial.print(" ==> ");
		Serial.print(press_type);
		Serial.print("\n");
	}
	*/

	/*
	 * all the part below is temporary
	 */
	if (digitalRead(BUTTON1_GPIO_PIN) == 1) {
		// change seq
		albero_state = ALBERO_STATE_NORMAL;
		push_counter++;
	}
	if (digitalRead(BUTTON2_GPIO_PIN) == 1) {
		// stop/play music
		albero_state = ALBERO_CYCLIC_SEQ_STATE;
		new_state = true;
		seq_start = millis();
	}

	if (push_counter >= 2) {
		seq_idx = (seq_idx + 1) % SEQ_COUNT;
		push_counter = 0;
		if (seq_idx == 1) {
			set_random_stars();
		}
		if (seq_idx == 3) {
			snake_set_color(rgb_color(rand() % 40, rand() % 40, rand() % 40));
		}
		if (seq_idx == 6) {
			struct rgb colors = {
				.red = rand() % 40,
				.green = rand() % 40,
				.blue = rand() % 40,
			};
			queue_light_set_color(colors);
		}
	}

	if (seq_idx == 5) {
		light_sequence[seq_idx].period = random(1, 100);
	}

	switch (albero_state) {
	case ALBERO_CYCLIC_SEQ_STATE:
		seq_dur = millis() - seq_start;
		if (seq_dur >= 10000) {
			seq_idx = rand() % SEQ_COUNT;
			seq_start = millis();
			new_state = true;
		}
		if (seq_idx == 1) {
			if (new_state) {
				set_random_stars();
				new_state = false;
			}
		}
		if (seq_idx == 3) {
			if (new_state) {
				struct snake_config snake_cfg;

				snake_cfg.reset_colors = random(0, 1);
				snake_set_config(snake_cfg);
				snake_set_color(rgb_color(rand() % 40, rand() % 40, rand() % 40));
				new_state = false;
			}
		}
		if (seq_idx == 4) {
			if (new_state) {
				struct rgb colors = {
					.red = rand() % 40,
					.green = rand() % 40,
					.blue = rand() % 40,
				};
				func_seq_rand_color(colors);
				new_state = false;
			}
		}
		if (seq_idx == 6) {
			if (new_state) {
				struct rgb colors = {
					.red = rand() % 40,
					.green = rand() % 40,
					.blue = rand() % 40,
				};
				queue_light_set_color(colors);
				new_state = false;
			}
		}
		break;
	}

	light_sequence[seq_idx].update(&tree_colors, NULL);
	ledStrip.write(tree_colors.colors, tree_colors.len);

	delay(light_sequence[seq_idx].period);
}
