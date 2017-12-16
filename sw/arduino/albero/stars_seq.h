#ifndef STARTS_SEQ_H
#define STARTS_SEQ_H

#include "color_seq.h"

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

void stars_init(void);
void stars_update(struct color_seq *col, void *data);

#endif
