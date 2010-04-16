#ifndef GAME_BOARD_H
#define GAME_BOARD_H


#include "cocoos.h"
#include "brick_game_defines.h"
#include "ball.h"


#define LEFT_BORDER		0
#define RIGHT_BORDER	((uint16_t) 299)
#define TOP_BORDER		0
#define BOTTOM_BORDER	((uint16_t) 299)
#define TABLE_WIDTH		((RIGHT_BORDER - LEFT_BORDER) + 1)


#define N_ROWS			10
#define N_COLUMNS		10
#define BRICK_HEIGHT	6
#define BRICK_WIDTH		( ( TABLE_WIDTH ) / N_COLUMNS )

#define DELETED			0
#define VISIBLE			1




extern  uint8_t bricks[N_ROWS][N_COLUMNS];
extern  uint16_t brickBottoms[N_ROWS];
extern  uint16_t brickRightBorders[N_COLUMNS];
extern  uint16_t brickLeftBorders[N_COLUMNS];
extern 	uint16_t brickTops[N_ROWS];
void game_board_InitBricks(void);
Bool game_board_isBallInBrickRegion( Ball *ball );
Bool game_board_DeleteIfVisible( uint8_t row, uint8_t column );
BallHits game_board_GetBorderHit(Position ballPos);
BallHits game_board_GetBrickHit( Ball *ball );
void game_board_getBrickInfo( uint8_t *brickInfo );


#endif
