/*
 *  playerCommon.h
 *  Reversi
 *
 *  Created by Ryo OKAMOTO
 *  Copyright (c) All rights reserved.
 *
 */

void	makeNextList( int turn );
int	    effectiveness( int, int, int, int );
void	getStateIndexDef( int *, int *, int );
int     IsNotInNext( int, int);
