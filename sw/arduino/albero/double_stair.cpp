#include "double_stair.h"

#include <PololuLedStrip.h>

#define DOUBLE_STAIR_START	0
#define DOUBLE_STAIR_RB		1
#define DOUBLE_STAIR_RG		2
#define DOUBLE_STAIR_GR		3
#define DOUBLE_STAIR_GB		4
#define DOUBLE_STAIR_BR		5
#define DOUBLE_STAIR_BG		7

#define DEFAULT_DOUBLE_STAIRS_CONFIG_MAX_VALUE	50
#define DEFAULT_RED_VAL		50		
#define DEFAULT_GREEN_VAL	0
#define DEFAULT_BLUE_VAL	0

struct double_stairs {
	int state;
	struct rgb color;
	struct double_stairs_config config;
};

static int ds_state;
static struct double_stairs_config ds_config;
static struct double_stairs ds;

static struct rgb base_color =
{
	.red = DEFAULT_RED_VAL,
	.green = DEFAULT_GREEN_VAL,
	.blue = DEFAULT_BLUE_VAL,
};

void double_stair_init()
{
	ds_config.max_value = DEFAULT_DOUBLE_STAIRS_CONFIG_MAX_VALUE;
	ds_config.red_default = DEFAULT_RED_VAL;
	ds_config.green_default = DEFAULT_GREEN_VAL;
	ds_config.blue_default = DEFAULT_BLUE_VAL;

	ds.state = DOUBLE_STAIR_START;
	ds.config = ds_config;
}

/*
void double_stairs_led(int *state, struct rgb *color)
{
	switch (&state) {
	case DOUBLE_STAIR_START:
		color = base_color;
		if (base_color.red > 0) {
			&state = DOUBLE_STAIR_RB;
		} else if (base_color.green > 0) {
			&state = DOUBLE_STAIR_GB;
		} else if (base_color.blue > 0) {
			&state = DOUBLE_STAIR_BG;
		}
		break;
	case DOUBLE_STAIR_RB:
		break;
	case DOUBLE_STAIR_RG:
		break;
	case DOUBLE_STAIR_GR:
		break;
	case DOUBLE_STAIR_GB:
		break;
	case DOUBLE_STAIR_BR:
		break;
	case DOUBLE_STAIR_BG:
		break;
	default:
		break;
	}
}
*/

void double_stair_led(struct double_stairs *ds)
{
	switch (ds->state) {
	case DOUBLE_STAIR_START:
		ds->color.red = ds->config.max_value;
		ds->state = DOUBLE_STAIR_RB;
		break;
	case DOUBLE_STAIR_RB:
		// temporary, just to test the sequence
		if (ds->color.red == 0) {
			ds->state = DOUBLE_STAIR_BG;
		} else {
			ds->color.red--;
			ds->color.blue++;
		}
		break;
	case DOUBLE_STAIR_BG:
		// temporary, just to test the sequence
		if (ds->color.blue == 0) {
			ds->state = DOUBLE_STAIR_GR;
		} else {
			ds->color.blue--;
			ds->color.green++;
		}
		break;
	case DOUBLE_STAIR_GR:
		// temporary, just to test the sequence
		if (ds->color.green == 0) {
			ds->state = DOUBLE_STAIR_RB;
		} else {
			ds->color.green--;
			ds->color.red++;
		}
		break;
	default:
		break;
	}
}

void double_stair_update(struct color_seq *col, void *data)
{
	int i;
	struct rgb color;

	double_stair_led(&ds);
	for (i = 0; i < col->len; ++i) {
		col->colors[i] = rgb_color(ds.color.red, ds.color.green, ds.color.blue);
	}
}
