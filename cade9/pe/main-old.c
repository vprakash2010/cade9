#include <io.h>
#include <interrupt.h>
#include <inttypes.h>
#include <stdlib.h>
#include "cocoos.h"
#include "uart.h"
#include "ball.h"
#include "racket.h"
#include "gameBoard.h"
#include "brick_game_defines.h"
#include "clock.h"




#define BALL_TID_OFFSET	2


/* The balls */
static Ball *balls[ 5 ];
static uint8_t nBalls;



/* The racket */
static Racket *g_racket;


/* Events */
os_event_type *evRacketMove;

static os_event_type *evBallMove;
//static os_event_type *evBrickHit;





static void system_init(void);
static void CreateBall( Position ballPos );
static void SignalRacketMove( void );
static BallHits GetBallHit(Ball *ball, Racket *racket);
static Ball* getCurrentBall( void );


static int ball_task(void)
{
	OS_BEGIN;	
	/*for (;;)
	{*/
		/* Get a pointer to the current ball */
		static Ball *currentBall;
        currentBall = getCurrentBall();

		/* Get a pointer to the ball mutex */
		static os_sem_type *ballMutex;
        ballMutex = ball_GetMutexRef( currentBall );

		/* Hold the mutex and move the ball */
		OS_WAIT_SEM( ballMutex );
		ball_moveBall( currentBall );
		OS_SIGNAL_SEM( ballMutex );
		
		/* Signal the ball move event */
		OS_SIGNAL_EVENT( evBallMove );
		
		/* Wait 20ms */
		OS_WAIT_TICKS( 20 );
	//}

	OS_END;

	return 0;
}


static int hit_detection_task(void)
{
	OS_BEGIN;	
	/*for (;;)
	{*/
		OS_WAIT_SINGLE_EVENT( evBallMove );
		
		/* Calculate the ball index from the task id */
		static uint8_t ballTid;
        ballTid = os_event_get_signaling_tid( evBallMove ) - BALL_TID_OFFSET;

		static os_sem_type *mutex;
        mutex = ball_GetMutexRef( balls[ ballTid ] );

		/* The ball has moved, find out if it hit a border, the racket or a brick */
		OS_WAIT_SEM( mutex );
		static Ball *ball;
        ball = ball_CreateBallCopy( balls[ ballTid ] );
		OS_SIGNAL_SEM( mutex );

		static os_sem_type *racketMutex;
        racketMutex = racket_GetMutexRef( g_racket );

		OS_WAIT_SEM( racketMutex );
		static Racket *racket;
        racket = racket_CreateRacketCopy( g_racket );
		OS_SIGNAL_SEM( racketMutex );

		static BallHits ballHit = 0;
		ballHit = GetBallHit(ball, racket);
		
		ball_DeleteBall( ball );
		racket_DeleteRacket( racket );		

		if ( ballHit != BALL_HIT_NO_HIT ) {
			/* Get mutex */
			OS_WAIT_SEM( mutex );

			ball_changeDirection( balls[ ballTid ], ballHit );

			/* Release mutex */
			OS_SIGNAL_SEM( mutex );	
			
		}

//	}
	OS_END;

	return 0;
}



static int racket_task(void)
{
	OS_BEGIN;
	/*for (;;)
	{*/
		OS_WAIT_SINGLE_EVENT(evRacketMove);
		
		/* Get move command */
		static RacketMoves racketMove = RACKET_MOVE_NO_MOVE;
		uart_readChar((uint8_t*)&racketMove);

		static os_sem_type *racketMutex;
        racketMutex = racket_GetMutexRef( g_racket );

		/* Get mutex */
		OS_WAIT_SEM(racketMutex);

		/* Move racket */
		racket_moveRacket(g_racket, racketMove);

		/* Release mutex */
		OS_SIGNAL_SEM(racketMutex);
		
	//}
	OS_END;
	return 0;
}


static int update_display_task(void)
{
	OS_BEGIN;
/*	for (;;)
	{*/
		OS_WAIT_TICKS( 30 );
        Position racketPosition = racket_GetPosition( g_racket );
		uint8_t brickInfo[ 13 ] = {0,0,0,0,0,0,0,0,0,0,0,0,0};
		game_board_getBrickInfo( brickInfo );

		static uint8_t data[ 38 ];
        static uint8_t nBytesToSend;
        nBytesToSend = 1 + 4 * nBalls + 13 + 4;
		data[ 0 ] = nBytesToSend - 1;
        data[ 1 ] = ( racketPosition.x & 0xff00 ) >> 8;
	    data[ 2 ] = racketPosition.x & 0x00ff ;
	    data[ 3 ] = ( racketPosition.y & 0xff00 ) >> 8;
	    data[ 4 ] = racketPosition.y & 0x00ff;
        static uint8_t idx = 0;
        for ( idx = 5; idx < 18; ++idx) {
            data[ idx ] = brickInfo[ idx - 5 ];
		}
        static Ball *currentBall;
        static Position ballPosition;
        static uint8_t* ptr;
        ptr = &data[ 18 ];
        for ( idx = 0; idx < nBalls; ++idx ) {
            currentBall = balls[ idx ];
            OS_WAIT_SEM( ball_GetMutexRef( currentBall ) );
		    ballPosition = ball_GetPosition( currentBall );
		    OS_SIGNAL_SEM( ball_GetMutexRef( currentBall ) );
            *ptr++ = ( ballPosition.x & 0xff00 ) >> 8;
		    *ptr++ =   ballPosition.x & 0x00ff;
		    *ptr++ = ( ballPosition.y & 0xff00 ) >> 8;
		    *ptr++ =   ballPosition.y & 0x00ff;
        }
		
		uart_write( data, nBytesToSend );
        while(1);
	//}

	OS_END;
	return 0;
}



int main(void)
{
	system_init();
	os_init();
	uart_init( SignalRacketMove );
	nBalls = 0;

	Position ballPos;
	ballPos.x = 47;
	ballPos.y = 233;
	CreateBall( ballPos );

    ballPos.x = 100;
	ballPos.y = 150;
	CreateBall( ballPos );

    ballPos.x = 100;
	ballPos.y = 220;
	CreateBall( ballPos );

	g_racket = racket_init();
	game_board_InitBricks();
		
	evRacketMove = os_create_event();
	evBallMove = os_create_event();
	
	os_task_create( racket_task, 1 );
	os_task_create( hit_detection_task, 2 );	
	os_task_create( ball_task, 3 );
    os_task_create( ball_task, 4 );
    os_task_create( ball_task, 5 );
	os_task_create( update_display_task, 6 );

	/* Setup clock with 1 ms tick */
    clock_init(1);

	os_start();
    return 0;
}



static void system_init(void)
{
   DDRB=0xff;
   PORTB=0xff;
   DDRA=0x00;
   PORTA=0xff;
}




static BallHits GetBallHit(Ball *ball, Racket *racket)
{
	BallHits hit = BALL_HIT_NO_HIT;
    do {
        hit = game_board_GetBorderHit( ball_GetPosition( ball ) );
	    if ( hit != BALL_HIT_NO_HIT )
	        break;
    
        hit = racket_GetRacketHit(ball_GetPosition( ball ), racket);
	    if ( hit != BALL_HIT_NO_HIT )
		    break;
	
	    hit = game_board_GetBrickHit(ball);
	    if ( hit != BALL_HIT_NO_HIT )
		    break;
	} while ( 0 );
    return hit;
}


static void CreateBall( Position ballPos ) {
	balls[ nBalls ] = ball_init( ballPos );
	nBalls++;
}

static void SignalRacketMove( void ) {
    OS_INT_SIGNAL_EVENT(evRacketMove);
}

static Ball* getCurrentBall( void ) {
	uint8_t currentTid = OS_GET_TID();
	uint8_t ballIndex = currentTid - BALL_TID_OFFSET;
	return balls[ ballIndex ];
}



