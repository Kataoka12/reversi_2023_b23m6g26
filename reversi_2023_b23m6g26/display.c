/*
 *  display.c
 *  Reversi
 *
 *  Created by Ryo OKAMOTO
 *  Copyright (c) All rights reserved.
 *
 *	表示関係ルーチン
 */

#include <stdio.h>		/* printf の warning を避けるため	*/
#include <stdlib.h>		/* rand の warning を避けるため	*/

#include "display.h"	/* このファイルの関数宣言用 */

/* main.c で宣言している外部変数を参照するために必要 */
extern int state[8][8];


/*--------------------------------
    コマ数をチェックする
 ---------------------------------*/
void checkState( int *aCount, int *bCount )
/*【備考】Aさん，Bさんの２つの値を返したいので CallByReference にしてある */
{
    int i, j;
    
    for(i=0; i<=7; i++){
        for(j=0; j<=7; j++){
            /* 値が 1 か 2 であればカウントする */
            switch( state[i][j] ){
                /*【注意】ポインタ変数なのでポインタが指す値に+1する*/
                case 1:	(*aCount)++;	break;
                case 2:	(*bCount)++;	break;            
            }
        }
    }
}

/*--------------------------------
    局面を画面表示する
 ---------------------------------*/
void showState()
{
    int i, j;
    int	aCount=0, bCount=0;
    
    /* 盤面の状態を表示する */
    for(i=0; i<=7; i++){
        for(j=0; j<=7; j++){
            if(state[i][j] == 1){
                printf("● ");  //printf("○●") 
            }else if(state[i][j] == 2){
                printf("○ ");
            }else{
                printf("- ");
            }
        }
        printf("\n");
    }

    /* 全体をカウントして各の個数を表示 */
    checkState(&aCount, &bCount);  //上の関数を呼び出す
    printf("A:%2d, B:%2d, Total:%2d\n\n",aCount, bCount, aCount+bCount );
    
}
//void showState()
//{
//    int i, j;
//    int	aCount=0, bCount=0;
//    
//    /* 盤面の状態を表示する */
//    for(i=0; i<=7; i++){
//        for(j=0; j<=7; j++) printf("%d ", state[i][j]);        
//        printf("\n");
//    }
//    
//    /* 全体をカウントして各の個数を表示 */
//    checkState(&aCount, &bCount);  //上の関数を呼び出す
//    printf("A:%2d, B:%2d, Total:%2d\n\n",aCount, bCount, aCount+bCount );
//    
//}