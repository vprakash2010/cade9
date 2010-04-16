#ifndef _BRICK_GAME_DEFS_
#define _BRICK_GAME_DEFS_
#include <inttypes.h>

typedef enum ballhits BallHits;

enum ballhits {
		BALL_HIT_NO_HIT,
		BALL_HIT_BOTTOM_BORDER,
		BALL_HIT_TOP_BORDER,
		BALL_HIT_RIGHT_BORDER,
		BALL_HIT_LEFT_BORDER,
		BALL_HIT_RACKET,
		BALL_HIT_BRICK_BOTTOM,
		BALL_HIT_BRICK_TOP,
		BALL_HIT_BRICK_RIGHT_BORDER,
		BALL_HIT_BRICK_LEFT_BORDER
};


typedef struct {
		uint16_t	x;
		uint16_t	y;
		} Position;


#endif
