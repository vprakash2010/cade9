#include "racket.h"
#include "gameBoard.h"
#include "brick_game_defines.h"
#include <stdlib.h>


struct racket {
		Position	 position;
		uint8_t		 length;
		uint8_t		 speed;
		os_sem_type* mutex;
		};



Racket* racket_init( void ) {
	Racket *racket = ( Racket* )malloc( sizeof( Racket ) );
	racket->position.x = 130;
	racket->position.y = 240;
	racket->speed = 30;
	racket->length = 38;
	racket->mutex = os_create_sem(1);
	return racket;
}




Position racket_moveRacket(Racket *racket, RacketMoves racketMove) {
	switch (racketMove) {
		case RACKET_MOVE_LEFT:
			if ( racket->position.x <= (LEFT_BORDER + racket->length) )
				racket->position.x = LEFT_BORDER;
			else
				racket->position.x -= racket->speed;
			
			break;

		case RACKET_MOVE_RIGHT:			
			if ((racket->position.x + racket->length) >= (RIGHT_BORDER - racket->length) )
				racket->position.x = RIGHT_BORDER - racket->length;
			else
				racket->position.x += racket->speed;
			
			break;

		default:
			/* Illegal move command: do nothing */
			break;
	}
	return racket->position;	
}




Position racket_getRacketPos(const Racket *racket) {
	return racket->position;
}



BallHits racket_GetRacketHit(Position ballPos, Racket *racket) {
	BallHits racketHit = BALL_HIT_NO_HIT;
	Bool bBallAtRacketTop 		= ( ( ballPos.y + BALL_HEIGHT ) >= racket->position.y );
	Bool bBallAboveRacketBottom = ( ( ballPos.y + BALL_HEIGHT ) < ( racket->position.y + 6 ) );
	Bool bBallRightOfRacketLeft = ( ( ballPos.x + BALL_WIDTH  ) >= racket->position.x );
	Bool bBallLeftOfRacketRight = (   ballPos.x <= ( racket->position.x + racket->length ) );

	/* Check if ball at racket */
	if ( bBallAtRacketTop       && bBallAboveRacketBottom && 
	     bBallRightOfRacketLeft && bBallLeftOfRacketRight )
    	racketHit = BALL_HIT_RACKET;
   
	return racketHit;
}



Position racket_GetPosition( Racket *racket ) {
	return racket->position;
}


Racket* racket_CreateRacketCopy( Racket* racket ) {
	Racket *r = (Racket*)malloc(sizeof(Racket));
	*r = *racket;
	return r;
}

void racket_DeleteRacket( Racket *racket ) {
	free( racket );
}


os_sem_type *racket_GetMutexRef(Racket* racket) {
	return racket->mutex;
}
