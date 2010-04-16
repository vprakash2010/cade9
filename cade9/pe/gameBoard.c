#include "cocoos.h"
#include "gameBoard.h"
#include "uart.h"

#define BRICK_DELETE_MESSAGE_HEADER		59

uint8_t bricks[N_ROWS][N_COLUMNS];
uint16_t brickBottoms[N_ROWS];
uint16_t brickTops[N_ROWS];
uint16_t brickRightBorders[N_COLUMNS];
uint16_t brickLeftBorders[N_COLUMNS];

static BallHits GetBrickBottomHit(Ball *ball, uint8_t *row, uint8_t *column);
static BallHits GetBrickTopHit(Ball *ball, uint8_t *row, uint8_t *column);
static BallHits GetBrickRightBorderHit(Ball *ball, uint8_t *row, uint8_t *column);
static BallHits GetBrickLeftBorderHit(Ball *ball, uint8_t *row, uint8_t *column);
static Bool DeleteBrickIfVisible( uint8_t row, uint8_t column, uint8_t *hRow, uint8_t *hColumn );
static Bool DeleteBrick(Ball *ball, uint8_t row, uint8_t column, uint8_t *hRow, uint8_t *hColumn);





void game_board_InitBricks(void)
{
	uint8_t row;
	uint8_t column;
		
	for(row = 0; row < N_ROWS; row++)
	{
		for (column = 0; column < N_COLUMNS; column++)
		{
			bricks[row][column] = VISIBLE;
			brickRightBorders[column] = ( column + 1 ) * BRICK_WIDTH - 1;
			brickLeftBorders[column] =  column * BRICK_WIDTH;
		}
		brickBottoms[row] = 151 - row * BRICK_HEIGHT;
		brickTops[row] = 146 - row * BRICK_HEIGHT;
	}		
}



Bool game_board_isBallInBrickRegion( Ball *ball )
{
	Position ballpos = ball_GetPosition( ball );
	Bool bBelowBottom = ( brickBottoms[0] < ballpos.y );

	Bool bAboveTop = ( (ballpos.y + BALL_HEIGHT) < ( brickTops[N_ROWS-1] ) );

	/* Check if ballpos is outside brick area */

	if ( bBelowBottom || bAboveTop )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
	
	return TRUE;	
}




Bool game_board_DeleteIfVisible( uint8_t row, uint8_t column )
{
	if ( bricks[ row ][ column ] != DELETED )
	{
		bricks[ row ][ column ] = DELETED;		
		return TRUE;
	}
	else
		return FALSE;
}


BallHits game_board_GetBorderHit(Position ballPos)
{  
	/* Check if ball at left or right border */
	if (ballPos.x >= (RIGHT_BORDER - BALL_WIDTH)) 
	   return BALL_HIT_RIGHT_BORDER;

	else if (ballPos.x <= LEFT_BORDER)
	   return BALL_HIT_LEFT_BORDER;	
   
	/* Check if ball at top or bottom border */
	else if ( ballPos.y >= (BOTTOM_BORDER - BALL_HEIGHT) )
	   return BALL_HIT_BOTTOM_BORDER;
	
	else if (ballPos.y <= TOP_BORDER)
	   return BALL_HIT_TOP_BORDER;
	
	return BALL_HIT_NO_HIT;
}


BallHits game_board_GetBrickHit( Ball *ball )
{
	uint8_t row;
	uint8_t column;
	BallHits brickHit;
	
	do 
	{
		brickHit = GetBrickBottomHit(ball, &row, &column);
		if ( brickHit == BALL_HIT_BRICK_BOTTOM )
			break;

		brickHit = GetBrickTopHit(ball, &row, &column);
		if ( brickHit == BALL_HIT_BRICK_TOP )
			break;

		brickHit = GetBrickRightBorderHit(ball, &row, &column);
		if ( brickHit == BALL_HIT_BRICK_RIGHT_BORDER )
			break;
	
		brickHit = GetBrickLeftBorderHit(ball, &row, &column);
		if ( brickHit == BALL_HIT_BRICK_LEFT_BORDER )
			break;

		brickHit = BALL_HIT_NO_HIT;
			
	} while (0);
		
	return brickHit;
}

static BallHits GetBrickBottomHit(Ball *ball, uint8_t *row, uint8_t *column)
{
	BallHits brickHit = BALL_HIT_NO_HIT;
	Position ballpos = ball_GetPosition( ball );
	uint8_t iRow;
	uint8_t hitRow;
	uint16_t hitColumn;

	if (( game_board_isBallInBrickRegion( ball ) == FALSE ) || ( ball_GetVertDirection( ball ) == DOWN ))
	{
		return BALL_HIT_NO_HIT;	
	}
		
	hitColumn = ballpos.x / BRICK_WIDTH;
	
	for (iRow = 0; iRow < N_ROWS; iRow++)
	{
		if ( ( ballpos.y <= brickBottoms[iRow] ) && ( ballpos.y > brickTops[iRow] ))
		{
			if ( DeleteBrickIfVisible( iRow, hitColumn, &hitRow,(uint8_t*) &hitColumn ) )
			{
				brickHit = BALL_HIT_BRICK_BOTTOM;
				break;
			}
		}
	}	
		
	if ( brickHit != BALL_HIT_NO_HIT )
	{
		*row = hitRow;
		*column = hitColumn;
	}

	return brickHit;
}




static BallHits GetBrickTopHit(Ball *ball, uint8_t *row, uint8_t *column)
{
	BallHits brickHit = BALL_HIT_NO_HIT;
	Position ballpos = ball_GetPosition( ball );
	/* Check if the ball hit a brick top */
	uint8_t iRow;
	uint8_t hitRow;
	uint16_t hitColumn;

	if (( game_board_isBallInBrickRegion( ball ) == FALSE ) || ( ball_GetVertDirection( ball ) == UP ))
	{
		return BALL_HIT_NO_HIT;	
	}
		
	hitColumn = ballpos.x / BRICK_WIDTH;
	
	for (iRow = 0; iRow < N_ROWS; iRow++)
	{
		if ( ( ( ballpos.y + BALL_HEIGHT ) >= brickTops[iRow] )   &&
		     ( ( ballpos.y + BALL_HEIGHT ) < brickBottoms[iRow] ) )
		{
			if ( DeleteBrickIfVisible( iRow, hitColumn, &hitRow, (uint8_t*) &hitColumn ) )
			{
				brickHit = BALL_HIT_BRICK_TOP;
				break;
			}
		}
	}	
	
	if ( brickHit != BALL_HIT_NO_HIT )
	{
		*row = hitRow;
		*column = hitColumn;
	}

	return brickHit;
}




static BallHits GetBrickRightBorderHit(Ball *ball, uint8_t *row, uint8_t *column)
{
	BallHits brickHit = BALL_HIT_NO_HIT;
	Position ballpos = ball_GetPosition( ball );
	uint8_t iColumn;
	uint8_t hitRow;
	uint8_t hitColumn;

	if (( game_board_isBallInBrickRegion( ball ) == FALSE ) || ( ball_GetHorDirection( ball ) == RIGHT ))
	{
		return BALL_HIT_NO_HIT;	
	}
	
	hitRow = ( brickBottoms[0] - ballpos.y ) / BRICK_HEIGHT;
	
	hitRow = ( hitRow > (N_ROWS-1) ? (N_ROWS-1) : hitRow );
		
	for (iColumn = 0; iColumn < (N_COLUMNS-1) ; iColumn++)
	{
		if ( ( ballpos.x <= brickRightBorders[iColumn] ) &&
		     ( ballpos.x > brickLeftBorders[iColumn] ) )
		{
			if ( DeleteBrick(ball, hitRow, iColumn, &hitRow, &hitColumn) )
			{
				brickHit = BALL_HIT_BRICK_RIGHT_BORDER;
				break;
			}
		}						
	}	
	

	if ( brickHit != BALL_HIT_NO_HIT )
	{
		*row = hitRow;
		*column = hitColumn;
	}

	return brickHit;
}




static BallHits GetBrickLeftBorderHit(Ball *ball, uint8_t *row, uint8_t *column)
{
	BallHits brickHit = BALL_HIT_NO_HIT;
	Position ballpos = ball_GetPosition( ball );
	uint8_t iColumn;
	uint8_t hitRow;
	uint8_t hitColumn;

	if (( game_board_isBallInBrickRegion( ball ) == FALSE ) || ( ball_GetHorDirection( ball ) == LEFT ))
	{
		return BALL_HIT_NO_HIT;	
	}
	
	hitRow = ( brickBottoms[0] - ballpos.y ) / BRICK_HEIGHT;
	
	hitRow = ( hitRow > (N_ROWS-1) ? (N_ROWS-1) : hitRow );
	for (iColumn = 1; ( iColumn < N_COLUMNS )  ; iColumn++)
	{
		if ( ( ( ballpos.x + BALL_WIDTH ) >= brickLeftBorders[iColumn] ) &&
		     ( ( ballpos.x + BALL_WIDTH ) < brickRightBorders[iColumn] ) )
		{
			if ( DeleteBrick(ball, hitRow, iColumn, &hitRow, &hitColumn) )
			{
				brickHit = BALL_HIT_BRICK_LEFT_BORDER;
				break;
			}
			
		}						
	}		
	
	if ( brickHit != BALL_HIT_NO_HIT )
	{
		*row = hitRow;
		*column = hitColumn;
	}

	return brickHit;
}


static Bool DeleteBrickIfVisible( uint8_t row, uint8_t column, uint8_t *hRow, uint8_t *hColumn )
{
	if ( game_board_DeleteIfVisible( row, column ) == TRUE )
	{
		*hRow = row;				
		*hColumn = column;
		return TRUE;
	}
	return FALSE;
}


static Bool DeleteBrick(Ball *ball, uint8_t row, uint8_t column, uint8_t *hRow, uint8_t *hColumn)
{
	if ( ball_GetVertDirection( ball ) == UP )
	{
		if ( row == 0 ) 
		{
			if ( DeleteBrickIfVisible( row, column, hRow, hColumn ) )
			{
				return TRUE;
			}			
			
		}
		else if ( row > 0 )
		{
			if ( DeleteBrickIfVisible( row - 1, column, hRow, hColumn ) )
			{
				return TRUE;
			}
			else if ( DeleteBrickIfVisible( row, column, hRow, hColumn ) )
			{
				return TRUE;
			}
			
		}
	}
	else /* ball.vertical_direction == DOWN */
	{
		if ( row == ( N_ROWS-1 ))
		{
			if ( DeleteBrickIfVisible( row, column, hRow, hColumn ) )
			{
				return TRUE;
			}
		}
		else if ( row < ( N_ROWS-1 ) )
		{				
			if ( DeleteBrickIfVisible( row + 1, column, hRow, hColumn ) )
			{
				return TRUE;
			}
			else if ( DeleteBrickIfVisible( row, column, hRow, hColumn ) )
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}


void game_board_getBrickInfo( uint8_t *brickInfo ) {
	uint8_t bit;
    
	for ( bit = 0; bit < 100; ++bit ) {
		brickInfo[ bit/8 ] |= (( bricks[ bit/10 ][ bit - (bit/10) * 10 ] ) << ( 7 - ( bit - (bit/8) * 8 ) ));
	}
}


