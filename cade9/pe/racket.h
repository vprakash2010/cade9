#ifndef RACKET_H
#define RACKET_H

#include "cocoos.h"
#include "brick_game_defines.h"


typedef struct racket Racket;






typedef enum {
		RACKET_MOVE_NO_MOVE,
		RACKET_MOVE_LEFT  = 100,
		RACKET_MOVE_RIGHT = 200
		} RacketMoves;




Racket* racket_init( void );
Position racket_moveRacket(Racket *racket, RacketMoves racketMove);
Position racket_getRacketPos(const Racket *racket);
BallHits racket_GetRacketHit(Position ballPos, Racket *racket);
Position racket_GetPosition( Racket *racket );
Racket* racket_CreateRacketCopy( Racket* racket );
void racket_DeleteRacket( Racket *racket );
os_sem_type *racket_GetMutexRef(Racket* racket);

#endif
