#include <string.h>
#include <stdlib.h>

#include <PololuLedStrip.h>

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
	Serial.println(value);
	Serial.println(button->duration);

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
				if (alb_buttons[i].btn_press.duration > 0)
					alb_buttons[i].btn_evt.press_event = EVT_BTN_PRESS_SHORT;
			}
		}
	}
}

struct color_seq {
	int len;
	rgb_color *colors;
};

struct light_seq {	
	void (*init)(void *data);
	void (*update)(struct color_seq *colors, void *data);
};

// seq stars
#define LED_NUMBER        100
#define MAX_STARS         20
#define MAX_BRIGHT        50
#define BRIGHT_INCREMENT  2
#define BRIGHT_DECREMENT  1

int current_stars;

enum star_state {
	STAR_START,
	STAR_ASC,
	STAR_DESC,
	STAR_END
};

struct star_seq {
	uint8_t bright;
	uint8_t bright_increment;
	uint8_t bright_decrement;
	enum star_state state;
};

uint8_t led_pins[] = {3, 5, 6, 9, 10, 11, 13};
uint8_t led_free[LED_NUMBER];
int led_bright[] = {0, 0, 0, 0, 0, 0, 0};

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

// seq rainbow
rgb_color hsvToRgb(uint16_t h, uint8_t s, uint8_t v)
{
	uint8_t f = (h % 60) * 255 / 60;
	uint8_t p = (255 - s) * (uint16_t)v / 255;
	uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
	uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
	uint8_t r = 0, g = 0, b = 0;
	switch((h / 60) % 6) {
		case 0: r = v; g = t; b = p; break;
		case 1: r = q; g = v; b = p; break;
		case 2: r = p; g = v; b = t; break;
		case 3: r = p; g = q; b = v; break;
		case 4: r = t; g = p; b = v; break;
		case 5: r = v; g = p; b = q; break;
	}

	return rgb_color(r / 6, g / 6, b / 6);
}

void rainbow_update(struct color_seq *col, void *data)
{
	if (!col)
		return;

	uint16_t time = millis() >> 2;

	for (uint16_t i = 0; i < col->len; i++) {
		byte x = (time >> 2) - (i << 3);
		col->colors[i] = hsvToRgb((uint32_t)x * 359 / 256, 255, 255);
	}
}


struct color_seq tree_colors;
struct light_seq light_sequence[2];
int seq_idx;
int push_counter;

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

	// color seq
	seq_idx = 0;
	tree_colors.len = 100;
	tree_colors.colors = (rgb_color *)malloc(100 * sizeof (rgb_color));

	light_sequence[0].update = rainbow_update;

	// light_seq[1].init = stars_init;
	stars_init();
	light_sequence[1].update = stars_update;
}

void loop() {
	/*
	if (button_task.execute)
		button_task.execute(NULL);

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
		push_counter++;
		if (push_counter >= 2) {
			seq_idx = (seq_idx + 1) % 2;
			push_counter = 0;
		}
	}
	if (digitalRead(BUTTON2_GPIO_PIN) == 1) {
		// stop/play music
	}

	light_sequence[seq_idx].update(&tree_colors, NULL);
	ledStrip.write(tree_colors.colors, tree_colors.len);

	delay(50);
}
