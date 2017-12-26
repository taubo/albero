#ifndef STARTS_SEQ_H
#define STARTS_SEQ_H

#include "color_seq.h"

enum star_state {
	STAR_START,
	STAR_ASC,
	STAR_DESC,
	STAR_END
};

struct stars_seq_params {
	int max_stars;
	uint8_t max_bright;
	uint8_t min_bright;
	uint8_t bright_increment;
	uint8_t bright_decrement;
	rgb_color fixed_led_color;
	rgb_color (*animated_led_color)(uint8_t bright);
};

struct star_seq {
	uint8_t bright;
	uint8_t bright_increment;
	uint8_t bright_decrement;
	enum star_state state;
};

void stars_init(void);
void stars_update(struct color_seq *col, void *data);
int stars_seq_set_params(const struct stars_seq_params *params);
void stars_get_default_params(struct stars_seq_params *params);

#endif
