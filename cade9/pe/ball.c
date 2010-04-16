#include "cocoos.h"
#include "ball.h"
#include "gameBoard.h"
#include <stdlib.h>

struct ball {
		Position position;
		uint8_t	horizontal_direction;
		uint8_t	horizontal_speed;
		uint8_t	vertical_direction;		
		uint8_t	vertical_speed;
		os_sem_type *mutex;
		};



Ball* ball_init( Position ballPos ) {
	Ball *ball = ( Ball* )malloc( sizeof(Ball) );
	ball->position = ballPos;
	ball->horizontal_direction = RIGHT;
	ball->vertical_direction = DOWN;
	ball->horizontal_speed = 1;
	ball->vertical_speed =   1;
	ball->mutex = os_create_sem(1);
	return ball;		
}





Position ball_moveBall(Ball *ball) {
	if (ball->horizontal_direction == RIGHT) {
		ball->position.x += ball->horizontal_speed;
		if ( ball->position.x >= ( RIGHT_BORDER - BALL_WIDTH ) ) {
			ball->position.x = RIGHT_BORDER - BALL_WIDTH;
		}
		
	}
    else {
		if ( ball->position.x <= (LEFT_BORDER + ball->horizontal_speed))
			ball->position.x = LEFT_BORDER;
		else
			ball->position.x-=ball->horizontal_speed;
	}
	
	if (ball->vertical_direction == UP) {
		if (ball->position.y <= (TOP_BORDER + ball->vertical_speed))
			ball->position.y = TOP_BORDER;
		else
		 ball->position.y -= ball->vertical_speed;
	}
	else {
		if ((ball->position.y + BALL_HEIGHT) >= (BOTTOM_BORDER - ball->vertical_speed))
			ball->position.y = BOTTOM_BORDER - BALL_HEIGHT;
		else
			ball->position.y+=ball->vertical_speed;
	}

	return ball->position;
}




Position ball_getBallPos(const Ball *ball) {
	return ball->position;
}




void ball_changeDirection(Ball *ball, BallHits ballHit) {
	switch (ballHit) {
		case BALL_HIT_RIGHT_BORDER:
        case BALL_HIT_BRICK_LEFT_BORDER:
			ball->horizontal_direction = LEFT;	
			break;

		case BALL_HIT_LEFT_BORDER:
        case BALL_HIT_BRICK_RIGHT_BORDER:
			ball->horizontal_direction = RIGHT;	
			break;

		case BALL_HIT_BOTTOM_BORDER:
        case BALL_HIT_BRICK_TOP:
			ball->vertical_direction = UP;	
			break;

		case BALL_HIT_TOP_BORDER:
        case BALL_HIT_BRICK_BOTTOM:
			ball->vertical_direction = DOWN;	
			break;

		case BALL_HIT_RACKET:
			if ( ball->vertical_direction == DOWN )
				ball->vertical_direction = UP;
			else
				ball->vertical_direction = DOWN;	
			break;

		
		default:
			break;
					
	}
}


os_sem_type *ball_GetMutexRef(Ball* ball) {
	return ball->mutex;
}

Ball* ball_CreateBallCopy( Ball* ball ) {
	Ball *temp = (Ball*)malloc( sizeof( Ball ) );
	*temp = *ball;
	return temp;
}


Position ball_GetPosition( Ball* ball ) {
	return ball->position;
}


uint8_t ball_GetVertDirection( Ball* ball ) {
	return ball->vertical_direction;	
}


uint8_t ball_GetHorDirection( Ball* ball ) {
	return ball->horizontal_direction;
}


void ball_DeleteBall( Ball *ball ) {
	free( ball );
}
