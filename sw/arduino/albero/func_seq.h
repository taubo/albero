#ifndef FUNC_SEQ_H
#define FUNC_SEQ_H

#include "color_seq.h"

void func_seq_rand_color(struct rgb col);
void func_seq_update(struct color_seq *colors, void *data);

#endif
