/*
 *  samplePlayer.c
 *  Reversi
 *
 *  Created by Ryo OKAMOTO
 *  Copyright (c) All rights reserved.
 *
 */

#include <stdio.h>           /* printf関数 の warning を避けるため */
#include <time.h>            /* time関数 の warning を避けるため   */
#include <stdlib.h>          /* rand関数 の warning を避けるため   */

#include "playerCommon.h"    /* プレーヤ用ユーティリティ関数 */
#include "samplePlayer.h"    /* このファイルの関数宣言用 */

/* main.c で宣言している外部変数を参照するために必要 */
extern    int state[8][8];
extern    int next [60][2];    /* 次候補リスト */  //[コマ番号][x,y], [0][0]にはnextに最初に格納されたマスのx座標, [0][1]にはそのy座標が格納されている

/*--------------------------------
    先手ルーチンのメイン
 ---------------------------------*/
void samplePlayer( int *x, int *y, int turn )
{
    int    i, j, num;
    
    /*【デバッグ】プレーヤ情報を表示 */
    printf("Player(%d) ●\n", turn);

    /* 次候補リストを -1 で初期化する*/
    for(i=0;i<=59;i++)
        for(j=0;j<=1;j++)
            next[i][j]=-1;
    
    /* プレーヤ番号を引数にして次候補リストを生成する */
    makeNextList( turn );
    
    /* 候補数をカウントする */
    num=0;
    while( next[num][0]!=-1 ) num++;

    /*【デバッグ】次候補リストの確認表示 */
    //printf("num = %d\n", num);
    //for(i=0;i<=59;i++)
    //    if(next[i][0]!=-1)
    //        printf("next[%d][0]=%d, next[%d][1]=%d\n", i, next[i][0], i, next[i][1]);

    /* 次手の候補がある場合 */
    if(num >= 1){

        //次の手(next)が角ならすぐ角に打つ手法. 検証の際には実行しない
        /*
        for(int i=0; next[i][0]!=-1; i++){
            if((next[i][0]==0 && next[i][1]==0) || (next[i][0]==0 && next[i][1]==7) || (next[i][0]==7 && next[i][1]==0) || (next[i][0]==7 && next[i][1]==7)){
                *x = next[i][0];
                *y = next[i][1];
                return;  //printf("Corner!");//exit(1);
            }
        }
        */

        /* 現在の時刻を種に 0 〜 num-1 までの乱数を発生させて次の手を決定する（秒単位なので1秒以下の連続実行だと同じ値になる） */
        srand((unsigned int)time(NULL));
        i = rand() % num;
        
        /*【デバッグ】選択状況を表示する*/
        //printf("doReverce i=%d, j=%d, Index:%d (in %d places)\n", next[i][0], next[i][1], i, num );

        /* 決定した手の座標を x,y にこの関数の結果として保存 */
        //i番目にnextに格納されたマスのx座標, y座標をx,yを代入している
        *x = next[i][0];    *y = next[i][1];
    
    } else {
    
        /* 置ける場所が無くパスするので -1 を結果として返す */
        *x = -1;    *y = -1;
        
    }
}
