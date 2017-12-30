#ifndef QUEUE_LIGHT_H
#define QUEUE_LIGHT_H

#include "color_seq.h"

void queue_light_init(void);
void queue_light_set_color(struct rgb color);
void queue_light_update(struct color_seq *col, void *data);

#endif
