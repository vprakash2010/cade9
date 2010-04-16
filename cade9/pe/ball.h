#ifndef BALL_H
#define BALL_H

#include "cocoos.h"
#include "brick_game_defines.h"

#define LEFT			0
#define RIGHT			1
#define UP			2
#define DOWN			3
#define BALL_WIDTH		6
#define BALL_HEIGHT		6

typedef struct ball Ball;





Ball* ball_init( Position ballPos );
Position ball_moveBall( Ball *ball );
Position ball_getBallPos(const Ball *ball);
void ball_changeDirection(Ball *ball, BallHits ballHit);
os_sem_type *ball_GetMutexRef(Ball *ball);
Ball* ball_CreateBallCopy( Ball *ball );
Position ball_GetPosition( Ball *ball );
uint8_t ball_GetVertDirection( Ball *ball );
uint8_t ball_GetHorDirection( Ball *ball );
void ball_DeleteBall( Ball *ball );


#endif
