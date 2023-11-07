/*
 *  playGame.c
 *  Reversi
 *
 *  Created by Ryo OKAMOTO
 *  Copyright (c) All rights reserved.
 *
 */
 
#include <stdio.h>		/* printf の warning を避けるため	*/
#include <stdlib.h>		/* exit の warning を避けるため	*/

#include "playGame.h"	/* このファイルの関数宣言用 */

#include "display.h"		/* 表示用のルーチン */
#include "samplePlayer.h"	/* サンプル・プレーヤのルーチン */
#include "b23m6g26r.h"

/* main.c で宣言している外部変数を参照するために必要 */
extern int state[8][8];
extern int next[60][2];

//再探索の回数を制限しておく。
int reExplore = 0;
int reExploreLimit = 5;

/*--------------------------------
    ゲームを進める（ゲーム進行のメイン）
 ---------------------------------*/
int proceedGame( int turn )
{    
    /* 差し手の座標 */
    int x, y;
    /* プレーヤ毎に交互に手を進めさせる */
    if( turn == 1 ){
    
        /*【重要】プレーヤ１（先手）に思考させる（結果は x, y に入る） */
        /* ☆ 課題としてここから呼ばれる関数を作成する！！ */
        //samplePlayer( &x, &y, turn );
        b23m6g26r( &x, &y, turn );  //遺伝的アルゴリズム用の相手
        //b23m6g26r_control( &x, &y, turn );  //自身で打ち手を決める関数
                
    } else {
        
        /*【重要】プレーヤ２（後手）に思考させる（結果は x, y に入る） */
        /* ※ 課題としてここから呼ばれる関数を作成する！！ */
    	//samplePlayer( &x, &y, turn );
        //b23m6g26r( &x, &y, turn );
        b23m6g26r_control( &x, &y, turn );  //自身で打ち手を決める関数
            
    }

    /* パス（x=-1, y=-1）でなければ，実際に手を差す */
    if( x != -1 )
        /* 実際にコマを置く */
        doReverce( x, y, turn );  //下の関数を呼び出す
    else 
        /* パスしたので -1 を返す */
        return -1;
    
    /* 正常に１手進んだ場合は0を返す */
    return 0; 
}

/*--------------------------------
    終局であるかどうかをチェックする
 ---------------------------------*/
int checkFinish()
{
    /* まだ終わりでない：res=0，終局：res=1 を返す */
    int	res=0;
    int i, j, p1=0, p2=0;
    
    /* 全てのコマを走査する */
    for(i=0; i<=7; i++){
        for(j=0; j<=7; j++){
            /* コマの値が 1 か 2 であれば各々カウントする */
            switch( state[i][j] ){
                /*【注意】ポインタ変数なのでポインタが指す値に+1する*/
                case 1:	p1++;	break;
                case 2:	p2++;
            }
        }
    }
    
    /* 終局条件:「全て埋まっている」or「全て同じ色のコマである」　*/
    if( p1+p2>=64 || p1==0 || p2==0 )	res=1;
        
    return res;
}

/*--------------------------------
	手を差して局面を変化させる
 ---------------------------------*/
void doReverce( int x, int y, int turn )
 /* x,y: 駒を置く座標，turn: プレーヤー番号	*/
{
    int  i, dist= 1, poss, count, check=0;
        
    /* ８方向の差分表現 */
    int def[8][2] = { {-1,-1}, {-1,0}, {-1,1}, {0,-1}, {0,1}, {1,-1}, {1,0}, {1,1} };
    
    /* 手駒を置く */
    state[x][y] = turn;

    /* 相手の駒を反転させる (8方向全てをチェック) */
    for( i=0; i<=7; i++){
        
        dist=1;
        
        /* 方向 i の反転が可能かをチェック */
        
        /* まず方向 i の側に相手の駒があるかをチェック */
        poss = 0;	count=1;
        if( state[x+def[i][0]][y+def[i][1]] != 0 && state[x+def[i][0]][y+def[i][1]] != turn ){
           
            /* さらに，その先に自分の駒があって挟めるかをチェック */
            
            /* 番からはみ出すか駒が無くなるまでチェック */
            while(	x+def[i][0]*count>=0 && x+def[i][0]*count<=7 &&
                    y+def[i][1]*count>=0 && y+def[i][1]*count<=7 &&
                    state[x+def[i][0]*count][y+def[i][1]*count]!=0 ){
                    
                /* 自分の駒があれば挟めると判断！ */
                if( state[x+def[i][0]*count][y+def[i][1]*count] == turn){
                    poss = 1;
                }
                count++;
            }
        }
        
        /* 相手の駒を反転させることができる場合 */
        if( poss == 1 ){

            /* 自分の駒に当たるまで反転を繰り返す */
            dist = 1;  //まずは1つ隣を調べる
            while(	state[x+def[i][0]*dist][y+def[i][1]*dist] != turn &&
                    state[x+def[i][0]*dist][y+def[i][1]*dist] != 0) {
                check=1;
                state[x+def[i][0]*dist][y+def[i][1]*dist] = turn;  //反転
                dist++;
            }
        }
    }
    
    /* コマが置けないエラーが発生した場合は異常終了させる */
    if(check==0){
    	printf("============= doReverce ERROR =============\n");
        exit(1);
    }
}


/*--------------------------------
    局面を初期化する
 ---------------------------------*/
void initState()
{
    int i, j;
    
    /* 最初の４コマを設定 */
    for(i=0; i<=7; i++)
        for(j=0; j<=7; j++)
            state[i][j]=0;
    
    /* 最初の４コマを設定 */
    state[3][3]=1;	state[4][3]=2;
    state[3][4]=2;	state[4][4]=1;

    /* 局面を表示 */
    showState();  //in"display.c"
}