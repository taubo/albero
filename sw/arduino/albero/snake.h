#ifndef SNAKE_SEQ_H
#define SNAKE_SEQ_H
#include "color_seq.h"

struct snake_config {
	uint8_t reset_colors;
};

void snake_init(void);
void snake_set_config(struct snake_config config);
void snake_update(struct color_seq *col, void *data);
void snake_set_color(rgb_color color);
// int stars_seq_set_params(const struct stars_seq_params *params);
// void stars_get_default_params(struct stars_seq_params *params);

#endif
