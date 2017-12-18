#ifndef COLOR_SEQ_H
#define COLOR_SEQ_H

#include <PololuLedStrip.h>

struct color_seq {
	int len;
	rgb_color *colors;
};

struct light_seq {	
	int period;
	void (*init)(void *data);
	void (*update)(struct color_seq *colors, void *data);
};

#endif
