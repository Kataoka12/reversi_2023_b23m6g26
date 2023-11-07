/*
 *  playerCommon.c
 *  Reversi
 *
 *  Created by Ryo OKAMOTO
 *  Copyright (c) All rights reserved.
 *
 */

#include <stdio.h>			/* printf の warning を避けるため	*/

#include "playerCommon.h"	/* このファイルの関数宣言用 */

/* main.c で宣言している外部変数を参照するために必要 */
extern int state[8][8];
extern int next [60][2];

/*--------------------------------
	次の手の局面リストの生成
 ---------------------------------*/
void makeNextList( int turn )
/* turn はプレイヤ番号：1=先手，2=後手，next は次候補リスト（最大数 60)　*/
 {
    //縦横, 横縦, nextの個数, blankの初期化用変数, 任意のマスを中心とした8方向の縦横, 任意のマスを中心とした8方向の横縦
    int i, j, count=0, k, iDef, jDef;
		
	/* 周囲の空き情報：上側左端を0として右向き下段に向かって 7 まで */
    int blank[8];
        
    /*【アルゴリズム】置ける場所を探す（評価値を与える関数は各プレーヤのファイルへ書く！） */
    //i==0は一番上の行, i==7は一番下の行, j==0は一番左の列, j==7は一番右の列
    for( i=0; i<=7; i++ ){
    
        for( j=0; j<=7; j++ ){
        
            /*【重要】相手の駒が置いてある場所を検出 */
            if( state[i][j]!=turn && state[i][j]!=0 ){
                            
                /* 空き情報(8方向)を0で初期化（開いていれば１） */
                for( k=0; k<=7; k++ ) blank[k]=0;

                /* 相手の駒の周囲が開いているかをチェック */
                if( state[i-1][j-1] == 0 && i!=0 ) blank[0]=1;	/* 左上 */
                if( state[i-1][j  ] == 0 && i!=0 ) blank[1]=1;	/* 真上 */
                if( state[i-1][j+1] == 0 && i!=0 ) blank[2]=1;	/* 右上 */
                if( state[i  ][j-1] == 0 && j!=0 ) blank[3]=1;	/* 左横 */
                if( state[i  ][j+1] == 0 && j!=7 ) blank[4]=1;	/* 右横 */
                if( state[i+1][j-1] == 0 && i!=7 ) blank[5]=1;	/* 左下 */
                if( state[i+1][j  ] == 0 && i!=7 ) blank[6]=1;	/* 真下 */
                if( state[i+1][j+1] == 0 && i!=7 ) blank[7]=1;	/* 右下 */
                
                /* 開いている場所に手を打てるかを順に調べる */
                for( k=0; k<=7; k++ ){
                    /* 空きがある場合 */
                    if( blank[k]==1 ){
                        /* 差し手の有効性と重複をチェックする */
                        if( effectiveness(i, j, k, turn) ){  //下の関数を呼び出す
                            
                            /* 方向番号からセル番号の差分を取得 */
                            getStateIndexDef(&iDef,&jDef,k);  //下の関数を呼び出す
                            
                            /* 重複して数えるので既に次候補リストnextに無い場合のみ追加する */
                            //各マスで8方向を確認しているため重複するのかも
                            if( IsNotInNext(i+iDef,j+jDef) ){  //下の関数を呼び出す
                                next[count][0]=i+iDef;
                                next[count][1]=j+jDef;
                                count++;
                            }
                        }
                    }
                }
            }
        }
    }
     //【デバッグ】候補数の表示
//     printf("COUNT = %d\n", count);
 }


/*--------------------------------
	相手の差し手の有効性を検証する
 ---------------------------------*/
//はさめるか否かの判定
int effectiveness( int i, int j, int k, int turn )
 /*【注意】i,j:「着目している相手の駒」の座標，k: 相手駒の周囲の空き方向 */
{
    int	res = 0, iDef, jDef, x, y;
    int	dist = 0, checkX, checkY;
    
    /* 差し手の座標を取得 */
    getStateIndexDef(&iDef,&jDef,k);  //下の関数を呼び出す
    x = i+iDef;	y=j+jDef;
    
    /* 差し手が盤面からはみ出す場合は却下 */
    if(x<0 || x>7 || y<0 || y>7) return res; 
    
    /*【アルゴリズム】挟めるかどうかを検証 */
    while(1){
        
        dist++;	/* 距離を広げる */
        
        /* 調査するセルの座標を取得 */
        checkX = i - iDef*dist;
        checkY = j - jDef*dist;
        
        /* 自分の駒があれば有効性が確定 (範囲に注意) */
        
        /* 盤をはみ出すか，自分の駒がなく空きセルがくれば置けない！ */
        if( 0>checkX || checkX>7 || 0>checkY || checkY>7 || state[checkX][checkY]==0 ) break;
        
        /* 自分の手駒で挟めることが判明！ */
        if( state[checkX][checkY]==turn ){
            res = 1;
            break;
        }
    
    }
    
    return res;
}

/*--------------------------------
	方向番号からセル番号の差分を取得
 ---------------------------------*/
void getStateIndexDef( int *iDef, int *jDef, int k )
{
    switch( k ){
        case 0:	 (*iDef)=-1; (*jDef)=-1;	break;	/* 左上 */
        case 1:	 (*iDef)=-1; (*jDef)= 0;	break;	/* 真上 */
        case 2:	 (*iDef)=-1; (*jDef)= 1;	break;	/* 右上 */
        case 3:	 (*iDef)= 0; (*jDef)=-1;	break;	/* 左横 */
        case 4:	 (*iDef)= 0; (*jDef)= 1;	break;	/* 右横 */
        case 5:	 (*iDef)= 1; (*jDef)=-1;	break;	/* 左下 */
        case 6:	 (*iDef)= 1; (*jDef)= 0;	break;	/* 真下 */
        case 7:	 (*iDef)= 1; (*jDef)= 1;			/* 右下 */
    }
}

/*--------------------------------
   next にあるかを確認（無い場合: 1）
---------------------------------*/
int IsNotInNext( int x, int y)
{
    int i=0;
        
    /* -1 の位置までリストを辿る */
    while( next[i][0]!=-1 ){
        
        /* リスト中にあるかを確認 */
        if( next[i][0]==x && next[i][1]==y ){
            return(0);
        }
        
        i++;
    }
    
    return(1);
}
