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
#include <math.h>

#include "playerCommon.h"    /* プレーヤ用ユーティリティ関数 */
#include "b23m6g26r.h"    /* このファイルの関数宣言用 */

#include "playGame.h"        //checkFinish関数を使うために必要
#include "display.h"         //checkState関数を使うために必要

/* main.c で宣言している外部変数を参照するために必要 */
//stateとnextはどこで書き換えても変更されるため注意
extern    int state[8][8];  //[x][y]はあくまでもx=行, y=列
extern    int next [60][2];    /* 次候補リスト */  //[コマ番号][x,y], [0][0]にはnextに最初に格納されたマスのx座標, [0][1]にはそのy座標が格納されている


int b23m6g26r_lookup_edge[8][8]={  //端を取るためのルックアップ
    {10,  -5, 10, 10, 10, 10,  -5, 10},
    {-5, -10, -5, -5, -5, -5, -10, -5},
    {10,  -5,  0,  0,  0,  0,  -5, 10},
    {10,  -5,  0,  0,  0,  0,  -5, 10},
    {10,  -5,  0,  0,  0,  0,  -5, 10},
    {10,  -5,  0,  0,  0,  0,  -5, 10},
    {-5, -10, -5, -5, -5, -5, -10, -5},
    {10,  -5, 10, 10, 10, 10,  -5, 10}
};
int b23m6g26r_lookup_center[8][8]={  //中心を取るためのルックアップ
    {0, 0, 0,  0,  0, 0, 0, 0},
    {0, 1, 1,  1,  1, 1, 1, 0},
    {0, 1, 5,  5,  5, 5, 1, 0},
    {0, 1, 5, 10, 10, 5, 1, 0},
    {0, 1, 5, 10, 10, 5, 1, 0},
    {0, 1, 5,  5,  5, 5, 1, 0},
    {0, 1, 1,  1,  1, 1, 1, 0},
    {0, 0, 0,  0,  0, 0, 0, 0}
};
int b23m6g26r_lookup_sakai[8][8]={  //酒井君のルックアップ
    {  4,-7,1,1,1,1,-7,  4},
    { -7,-7,2,2,2,2,-7, -7},
    {  1, 2,3,3,3,3, 2,  1},
    {  1, 2,3,4,4,3, 2,  1},
    {  1, 2,3,4,4,3, 2,  1},
    {  1, 2,3,3,3,3, 2,  1},
    { -7,-7,2,2,2,2,-7, -7},
    {  4,-7,1,1,1,1,-7,  4},
};
int b23m6g26r_inverse_lookup_sakai[8][8]={  //lookup_sakaiの逆
    { 0, 4, 3, 3, 3, 3, 4, 0},
    { 4, 4, 2, 2, 2, 2, 4, 4},
    { 3, 2, 1, 1, 1, 1, 2, 3},
    { 3, 2, 1,-7,-7, 1, 2, 3},
    { 3, 2, 1,-7,-7, 1, 2, 3},
    { 3, 2, 1, 1, 1, 1, 2, 3},
    { 4, 4, 2, 2, 2, 2, 4, 4},
    { 0, 4, 3, 3, 3, 3, 4, 0},

};
int b23m6g26r_lookup_sakai_corner[8][8]={  //lookup_sakaiの角の横のマスを軸に評価値の割り振り
    { 0,-7, 1, 2, 2, 1,-7, 0},
    {-7,-7, 2, 3, 3, 2,-7,-7},
    { 1, 2, 3, 3, 3, 3, 2, 1},
    { 2, 3, 3, 4, 4, 3, 3, 2},
    { 2, 3, 3, 4, 4, 3, 3, 2},
    { 1, 2, 3, 3, 3, 3, 2, 1},
    {-7,-7, 2, 3, 3, 2,-7,-7},
    { 0,-7, 1, 2, 2, 1,-7, 0},

};
int b23m6g26r_lookup_bros[8][8]={  //
    { 12,-7, 4, 1, 1, 4,-7, 12},
    {-7,-7, 3, 1, 1, 3,-7,-7},
    { 4, 3, 1, 2, 2, 1, 3, 4},
    { 1, 1, 2, 3, 3, 2, 1, 1},
    { 1, 1, 2, 3, 3, 2, 1, 1},
    { 4, 3, 1, 2, 2, 1, 3, 4},
    {-7,-7, 3, 1, 1, 3,-7,-7},
    { 12,-7, 4, 1, 1, 4,-7, 12},
};
//取っている角に近いマスほど大きな重みをつけるためのlookup. 評価関数用.
int lookup_ifTakeCorner[8][8]={
    { 0, 7, 6, 5, 5, 6, 7, 0},
    { 7, 5, 4, 3, 3, 4, 5, 7},
    { 6, 4, 2, 1, 1, 2, 4, 6},
    { 5, 3, 1, 0, 0, 1, 3, 5},
    { 5, 3, 1, 0, 0, 1, 3, 5},
    { 6, 4, 2, 1, 1, 2, 4, 6},
    { 7, 5, 4, 3, 3, 4, 5, 7},
    { 0, 7, 6, 5, 5, 6, 7, 0},
};

//randomか評価関数か
int mode = 1;  //0:random, 1:evaluation
//x, yの上書きをab_max_FirstTime()のみで行うためのフラグ
int fromAbmaxFirstTime = 0;
int fromAbmin = 0;
//FirstTimeと比較したコマの反転数
int StateNum_FirstTime = 0;
//評価関数内で反転させるコマに関する変数
int reverceState[60][2];  //評価関数内で反転させるコマの位置(x,y)の保存リスト.
int reverceStateNum = 0;  //評価関数内で反転させるコマの数
//次の打ち手のランダム選択用のリスト
int v_List[60][2];  //次の打ち手のランダム選択用にx, yの格納
int len_v_List = 0;  //v_Listの長さ. ランダムの範囲を決めるために使用
//探索の回数をカウント用の変数
int cnt_explore[101];  //探索の回数をカウントする. mini-max, alpha-beta法の比較に使用
int index_cnt_explore;  //cnt_exploreのインデックス
//評価値の確認用
int EVMaxMin[35][2];  //EvalVal_MaxAndMin. [][0]がmax, [][1]がmin. 評価関数の対応は上の関数の下（一番最後の行）.
int EVNew[35];  //EvalVal_New. 最新の評価値を格納する. EvalVal_MaxAndMinと比較して最大値と最小値を更新する
//int EvalVal_List[10000][5];  //評価関数の中身を表示するためのリスト.0は評価値の総和.FirstTimeから戻ってきた評価値の参照によって特定.間違っている可能性あり.
//int coef_tmp = 1;  //下の係数の決定までの仮
//b23m6g26r内で自身のturnを保存して他の関数内でturnの識別に使用
int myTurn;
int enemyTurn;
//
//各評価関数の係数. 遺伝的アルゴリズムでも利用
//講義での対戦で使用する係数
float coef_EvalFunc[30] = 

{  32.762024, 21.723972, -17.138981, 19.378000, 36.681965, 
  -18.018013, -40.259991, 45.684994, 42.850952, 89.012047, 
   67.133003, -21.459002, 30.828005, 38.318020, 4.774992, 
   -3.218011, -37.714996, 14.383005, -33.882999, 24.117018, 
    7.818003, 10.031981, 23.242002, -27.717999, 18.810972, 
   17.310999, -40.779953, -45.294987, -10.060990, -45.074913 };  //第二引数はこのファイルの一番下の評価関数一覧と対応



int playerNum = 0;  //coef_EvalFuncの第一引数. b23m6g26r.c内では変更しない
//
//
/*--------------------------------
    私のエージェントのルーチンのメイン
 ---------------------------------*/
void b23m6g26r( int *x, int *y, int turn )
{
    myTurn = turn;  //自身のturnの保存.
    if(myTurn==1)	enemyTurn=2;  //相手のturnの保存
            else enemyTurn=1;
    
    /*【デバッグ】プレーヤ情報を表示 */
    printf("Player(%d) ○\n", myTurn);

    /* 次候補リストを -1 で初期化する*/
    for(int i=0;i<=59;i++)
        for(int j=0;j<=1;j++)
            next[i][j]=-1;
    
    /* プレーヤ番号を引数にして次候補リストを生成する */
    makeNextList( myTurn );  //in"playerCommon.c"
    
    /* 候補数をカウントする */
    int nextNum=0;
    while( next[nextNum][0]!=-1 ) nextNum++;

    /* 次手の候補がある場合 */
    if(nextNum >= 1){

        int limit = 0;  //本番は深さ4
        int alpha=-100000, beta=100000;  //最初の評価値で上書きしてもらうために極端な値を入れておく
        
        //randomNum_global = nextNum;

        int EvalVal_next = 0;  //次の手の評価値
        //int EvalVal_i = 0;  //EvalVal_Listのi番目の評価値の総和
        //for(int i=0;i<1000;i++)  //EvalVal_Listの初期化
        //    for(int j=0;j<5;j++)
        //        EvalVal_List[i][j] = -1;

        len_v_List = 0;  //len_v_Listの初期化

        //先読みなしRandomAgent or 先読みあり評価関数 の選択
        int mode_RandomAgentOrEvaluation = 1;  //0:RandomAgent, 1:評価関数
        if(mode_RandomAgentOrEvaluation == 0){
            b23m6g26r_RandomAgent_FirstTime(x, y, turn, nextNum);
        }else{
            //printf("EvaluationFunc\n");
            EvalVal_next = b23m6g26r_ab_max_FirstTime(x, y, turn, limit-1, alpha, beta);
        }

        //打ち手のランダム選択. 同じ評価値の打ち手が複数ある場合
        if(len_v_List > 1){  //v_ListはFirstTime内で更新している
            int nextxyNum;
            srand((unsigned int)time(NULL));
            nextxyNum = rand() % len_v_List;  //len_v_List>1
            *x = v_List[nextxyNum][0];
            *y = v_List[nextxyNum][1];
        }

        //打つ手の評価値を参照, 表示.
        //上手くいっていない,2023/07/14
        //int i_max = 0;
        //
        //int enemyNum=0, myNum=0;
        //
        //ここは先攻後攻によって変化するのでmyTurn, enemyTurnで変化するように
        //
        //checkState(&enemyNum, &myNum);  //in"playerCommon.c"
        /*
        for(int i=0;EvalVal_List[i][0]!=-1;i++){
            //EvalVal_i = EvalVal_List[i][0] + EvalVal_List[i][1] + EvalVal_List[i][2] + EvalVal_List[i][3] + EvalVal_List[i][4];
            if(EvalVal_List[i][0] == EvalVal_next){  //[][0]は評価値の総和
                printf("num : all = corner, min, max, lookup\n");
                //printf("E[%d] : %d = %d, %d, %d, %d\n", i, EvalVal_List[i][0], EvalVal_List[i][1], EvalVal_List[i][2], EvalVal_List[i][3], EvalVal_List[i][4]);
                printf("%d : %d = %d, %d, %d, %d\n", (enemyNum+myNum)-4, EvalVal_List[i][0], EvalVal_List[i][1], EvalVal_List[i][2], EvalVal_List[i][3], EvalVal_List[i][4]);
                break;
            }
            //i_max = i;
        }
        */
        //表示
        //printf("num : all = corner, min, max, lookup\n");
                //printf("E[%d] : %d = %d, %d, %d, %d\n", i, EvalVal_List[i][0], EvalVal_List[i][1], EvalVal_List[i][2], EvalVal_List[i][3], EvalVal_List[i][4]);
        //printf("%d : %d = %d, %d, %d, %d\n", (enemyNum+myNum)-4, EvalVal_List[i][0], EvalVal_List[i][1], EvalVal_List[i][2], EvalVal_List[i][3], EvalVal_List[i][4]);
        //printf("i_max = %d\n", i_max+1);  //格納されている個数を表示
        //ab_max_FirstTime()から抜けた後のx, yの確認    //printf("b23m6g26r() after ab_max_FirstTime()\n");    printf("    &x, &y = %d, %d\n", &x, &y);    printf("    x, y = %d, %d\n", x, y);    printf("    *x, *y = %d, %d\n", *x, *y);
        //-----------------------------------------------
        //-----------------------------------------------

    } else {
        /* 置ける場所が無くパスするので -1 を結果として返す */
        printf("Player(%d) : Pass\n", myTurn);
        *x = -1;    *y = -1;

    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
//ab_max_FirstTime()でx,yをそのまま持ってこられるようにx,yを引数にしている
int b23m6g26r_EvaluationAndPolynomial(int *x, int *y, int turn, int next_save[60][2], int state_save[8][8], int EvalVal_Corner, int  EvalVal_Pass, int EvalVal_RevdNewMy, int lookup_RevdNewMy[8][8]){
    
    /////////////////////////////////////
    //評価値用変数
    int polynomialNumber=0;
    int polynomialNumber_best=-100000;
    int EvalVal_getStateMin, EvalVal_getStateMax;
    int EvalVal_lookup;  //lookup分の評価値
    int EvalVal_lookupBoard;  //lookup分の評価値
    int EvalVal_rateMore50Center;
    int EvalVal_weightIfTakeCorner;
    int EvalVal_rateMoreMyPerEnemyQuadrant;
    int EvalVal_leaveOne;
    int EvalVal_MoreAndLessSpaceAround;  //2つの評価値
    int EvalVal_MoreAndLessNextNum;  //2つの評価値
    int EvalVal_rate50Board;
    int EvalVal_overlapVertHoriDiag;  //２つの評価値
    int EvalVal_takeExtremeHalfWay;  //２つの評価値
    int EvalVal_noCount = 0;

    //コマを置く前のコマ数を保存. 多くとる、小さくとるなど複数の評価関数で利用する
    
    int myCount=0, enemyCount=0;
    int myCount_old=0, myCount_new=0;
    myCount_old = StateNum_FirstTime;  //現在（先読み前）での自身のコマ数をoldに保存

    //評価値による多項式の結果の比較によってx, yの決定
    //全ての候補の手の評価値を計算
    for(int i=0; next[i][0]!=-1; i++){  //nextCountがいらないver
        cnt_explore[index_cnt_explore]++;  //探索回数（=打ち手の数）のカウント
        
        //打つ手(next)が角か否か
        if(next[i][0]==0 && next[i][1]==0 || next[i][0]==0 && next[i][1]==7 || next[i][0]==7 && next[i][1]==0 || next[i][0]==7 && next[i][1]==7){
            if(turn == myTurn){
                EvalVal_Corner = EvalVal_Corner + 2000;
            }else{
                EvalVal_Corner = EvalVal_Corner - 2000;
            }
        }

        // 反転させるマスのリストを初期化
        for(int i=0;i<=59;i++){
            reverceState[i][0]=-1;
            reverceState[i][1]=-1;
        }
        reverceStateNum=0;  //反転させるマスの数を初期化

        //局面に子接点の手を仮に打つ
        b23m6g26r_EvalReverce(next[i][0], next[i][1], turn );  //反転させるマスのカウントも行う

        //初期化
        myCount=0;
        enemyCount=0;
        //コマを置いた後のコマ数を取得
        if(myTurn==1){
            checkState(&myCount, &enemyCount);
        }else{
            checkState(&enemyCount, &myCount);
        }
        myCount_new = myCount;
        
        /*
        if(enemyCount <= 0){
            EvalVal_noCount = EvalVal_noCount + 10000;
        }else if(myCount <= 0){
            EvalVal_noCount = EvalVal_noCount - 10000;
        }
        */
        
        //各評価値の計算
        //自身と相手の打ち手への評価関数. 各評価値を保存したいので最後にまとめて多項式計算
        if(fromAbmin==0){  //正
            //EvalVal_Cornerの正負は調整済み
            //EvalVal_Passの正負は調整済み
            EvalVal_lookup = b23m6g26r_lookup(*x, *y, turn, i);
            EvalVal_weightIfTakeCorner = b23m6g26r_weight_ifTakeCorner(*x, *y, turn, i);
            EvalVal_rateMoreMyPerEnemyQuadrant = b23m6g26r_rateMore_MyPerEnemy_quadrant(*x, *y, turn, i, state_save);
            EvalVal_leaveOne = b23m6g26r_leaveOne(*x, *y, turn, i);
            EvalVal_MoreAndLessSpaceAround = b23m6g26r_MoreAndLess_spaceAround(*x, *y, turn, i, 2);  //2つ評価値, mode = 0, 1, 2
            EvalVal_MoreAndLessNextNum = b23m6g26r_MoreAndLess_nextNum(*x, *y, turn, i, 2);  //2つ評価値値, mode = 0, 1, 2
            EvalVal_overlapVertHoriDiag = b23m6g26r_overlap_VertivalHorizontalDiagonal(*x, *y, turn, i, state_save, 2);  //2つ評価値値, mode = 0, 1, 2
        
        }else{  //負
            //EvalVal_Cornerの正負は調整済み
            //EvalVal_Passの正負は調整済み
            EvalVal_lookup = -1 * b23m6g26r_lookup(*x, *y, turn, i);
            EvalVal_weightIfTakeCorner = -1 * b23m6g26r_weight_ifTakeCorner(*x, *y, turn, i);
            EvalVal_rateMoreMyPerEnemyQuadrant = b23m6g26r_rateMore_MyPerEnemy_quadrant(*x, *y, turn, i, state_save);
            EvalVal_leaveOne = b23m6g26r_leaveOne(*x, *y, turn, i);
            EvalVal_MoreAndLessSpaceAround = b23m6g26r_MoreAndLess_spaceAround(*x, *y, turn, i, 2);  //2つ評価値, mode = 0, 1, 2
            EvalVal_MoreAndLessNextNum = b23m6g26r_MoreAndLess_nextNum(*x, *y, turn, i, 2);  //2つ評価値値, mode = 0, 1, 2
            EvalVal_overlapVertHoriDiag = b23m6g26r_overlap_VertivalHorizontalDiagonal(*x, *y, turn, i, state_save, 2);  //2つ評価値値, mode = 0, 1, 2        
        }
    
        //盤面、自身の打ち手のみへの評価関数
        EvalVal_getStateMin = b23m6g26r_getStateMin(*x, *y, turn, myCount_old, myCount_new);
        EvalVal_getStateMax = b23m6g26r_getStateMax(*x, *y, turn, myCount_old, myCount_new);
        EvalVal_lookupBoard = b23m6g26r_lookup_board(*x, *y, turn);
        EvalVal_rateMore50Center = b23m6g26r_rateMore50_center(*x, *y, turn);
        EvalVal_rate50Board = b23m6g26r_rate50_board(*x, *y, turn);
        EvalVal_takeExtremeHalfWay = b23m6g26r_take_ExtremeOrHalfWay(*x, *y, turn, myCount_old, myCount_new, 2);  //２つ評価値, mode = 0, 1, 2
        EvalVal_RevdNewMy = EvalVal_RevdNewMy + b23m6g26r_isReversed_newMyState(*x, *y, turn, i, lookup_RevdNewMy);  //ab_max_FirstTime(), ab_max(), ab_min()でも評価値を計算している
        //EvalVal_noCount

        //評価関数による多項式. 係数（[][n]）の掛け算も一部はここで行い、それ以外は各評価関数内で行う
        polynomialNumber = polynomialNumber + (EvalVal_Corner * coef_EvalFunc[0]);
        polynomialNumber = polynomialNumber + (EvalVal_Pass * coef_EvalFunc[1]);
        polynomialNumber = polynomialNumber + (EvalVal_getStateMin * coef_EvalFunc[2]);
        polynomialNumber = polynomialNumber + (EvalVal_getStateMax * coef_EvalFunc[3]);
        polynomialNumber = polynomialNumber + EvalVal_lookup;  //[][4] ~ [][9]はlookup()内
        polynomialNumber = polynomialNumber + EvalVal_lookupBoard;  //[][10] ~ [][15]はlookup_board()内
        polynomialNumber = polynomialNumber + (EvalVal_rateMore50Center * coef_EvalFunc[16]);
        polynomialNumber = polynomialNumber + (EvalVal_weightIfTakeCorner * coef_EvalFunc[17]);
        polynomialNumber = polynomialNumber + (EvalVal_rateMoreMyPerEnemyQuadrant * coef_EvalFunc[18]);
        polynomialNumber = polynomialNumber + (EvalVal_leaveOne * coef_EvalFunc[19]);
        polynomialNumber = polynomialNumber + EvalVal_MoreAndLessSpaceAround;  //[][20] ~ [][21]はMoreAndLess_spaceAround()内
        polynomialNumber = polynomialNumber + EvalVal_MoreAndLessNextNum;  //[][22] ~ [][23]はMoreAndLess_nextNum()内
        polynomialNumber = polynomialNumber + (EvalVal_rate50Board * coef_EvalFunc[24]);
        polynomialNumber = polynomialNumber + EvalVal_overlapVertHoriDiag;  //[][25] ~ [][26]はoverlap_VertivalHorizontalDiagonal()内
        polynomialNumber = polynomialNumber + EvalVal_takeExtremeHalfWay;  //[][27] ~ [][28]はtake_ExtremeOrHalfWay()内
        polynomialNumber = polynomialNumber + (EvalVal_RevdNewMy * coef_EvalFunc[29]);
        
        //EVNew(EvalVal_New)の更新
        EVNew[0] = EvalVal_Corner;
        EVNew[1] = EvalVal_Pass;
        EVNew[2] = EvalVal_getStateMin;
        EVNew[3] = EvalVal_getStateMax;
        //EvalVal_lookup;  //評価関数内
        //lookup_board;  //評価関数内
        EVNew[16] = EvalVal_rateMore50Center;
        EVNew[17] = EvalVal_weightIfTakeCorner;
        EVNew[18] = EvalVal_rateMoreMyPerEnemyQuadrant;
        EVNew[19] = EvalVal_leaveOne;
        //EvalVal_MoreAndLessSpaceAround;  //評価関数内
        //EvalVal_MoreAndLessNextNum;  //評価関数内
        EVNew[24] = EvalVal_rate50Board;
        //EvalVal_overlapVertHoriDiag;  //評価関数内
        //EvalVal_takeExtremeHalfWay;  //評価関数内
        EVNew[29] = EvalVal_RevdNewMy;  //isReversed_newMyState

        //EVMaxMinの更新
        b23m6g26r_Evaluation_MaxAndMin(turn);
        
        //最も良い評価値の更新
        if(polynomialNumber_best < polynomialNumber){
            //bestの更新
            polynomialNumber_best = polynomialNumber;
            
            //*x = next[i][0];
            //*y = next[i][1];

            //x, yの更新. FirstTimeから入ってきたらFirstTimeの代わりにx, yの更新をする
            if(fromAbmaxFirstTime == 1){  //ab_max_FirstTime()から入ったときのみx, yを更新する
                *x = next[i][0];
                *y = next[i][1];

                //v_List（ランダム選択用リスト）, len_v_Listの初期化
                len_v_List = 1;
                for(int i=0; i<60; i++){
                        v_List[i][0] = -1;  //xの初期化
                        v_List[i][1] = -1;  //yの初期化
                }
                //x, yのランダム選択用リストの更新
                v_List[len_v_List-1][0] = next[i][0];  //lenは1から, 要素番号は0から始めているので-1
                v_List[len_v_List-1][1] = next[i][1];
            }

            //評価値の中を参照できるように保存. 要素値==-1によって判定するので使用しない評価値は0として格納
            //for(int j=0;j<10000;j++){  //次に入れる要素番号の参照
            //    if(EvalVal_List[j][0]==-1){
            //        EvalVal_List[j][0] = polynomialNumber;
            //        EvalVal_List[j][1] = EvalVal_Corner;
            //        EvalVal_List[j][2] = EvalVal_getStateMin;
            //        EvalVal_List[j][3] = EvalVal_getStateMax;
            //        EvalVal_List[j][4] = EvalVal_lookup;
            //        break;
            //    }
            //}

        }else if((polynomialNumber_best == polynomialNumber) && (fromAbmaxFirstTime == 1)){
            len_v_List++;
            //x, yのランダム選択用の追加保存
            v_List[len_v_List-1][0] = next[i][0];
            v_List[len_v_List-1][1] = next[i][1];
        }

        //局面をもとに戻す. ローカルのsate_saveをもとにグローバルのstateを変更する
        b23m6g26r_state_SaveOrLoad_arg1Toarg2(state_save, state);  //state <- state_save : load
        b23m6g26r_next_SaveOrLoad_arg1Toarg2(next_save, next);  //next <- next_save : load
    }

    return polynomialNumber_best;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//評価関数.
//少なくとる評価関数
int b23m6g26r_getStateMin(int x, int y, int turn, int myCount_old, int myCount_new){
    
    int evaluation=0;
    float evaluation_float;
    int multiple=1000;
    int maxReverse = 12, minReverse = -12;
    int CountDifference=0;  //現在のコマ数と先読み後のコマ数の差
    
    CountDifference = myCount_new - myCount_old;
    
    //評価値の計算
    if(CountDifference >= maxReverse){
        evaluation = 0;
    }else if(CountDifference <= minReverse){
        evaluation = 1 * multiple;
    }else{
        //調整
        CountDifference = CountDifference + 13;
        minReverse = minReverse + 13;
        //評価値の計算
        evaluation_float = (float)(minReverse) / (float)CountDifference;  //1/CountDifference
        evaluation = (int)(evaluation_float * multiple);

    }
    return evaluation;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//多く取る評価関数
int b23m6g26r_getStateMax(int x, int y, int turn, int myCount_old, int myCount_new){

    int evaluation=0;
    float evaluation_float;
    int multiple=1000;
    int maxReverse = 12, minReverse = -12;
    int CountDifference=0;  //現在のコマ数と先読み後のコマ数の差

    CountDifference = myCount_new - myCount_old;
    
    //評価値の計算
    if(CountDifference >= maxReverse){
        evaluation = 1 * multiple;
    }else if(CountDifference <= minReverse){
        evaluation = 0;
    }else{
        //調整
        CountDifference = CountDifference + 13;
        maxReverse = maxReverse + 13;
        //評価値の計算
        evaluation_float = (float)(CountDifference) / (float)maxReverse;
        evaluation = (int)(evaluation_float * multiple);
    }
    return evaluation;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//打ち手へのlookupの値の計算用評価関数
int b23m6g26r_lookup(int x, int y, int turn, int nextI){
    //評価値
    int evaluation = 0;
    int evaluation_putStateEdge = 0;  //putState
    int evaluation_putStateCenter = 0;
    int evaluation_putStateSakai = 0;
    int evaluation_putStateInverseSakai = 0;
    int evaluation_putStateSakaiCorner = 0;
    int evaluation_putStateBros = 0;
    int evaluation_reverseEdge = 0;  //reverse
    int evaluation_reverseCenter = 0;
    int evaluation_reverseSakai = 0;
    int evaluation_reverseInverseSakai = 0;
    int evaluation_reverseSakaiCorner = 0;
    int evaluation_reverseBros = 0;
    //評価値の大きさの調整用
    int multiple_edge = 7;  //multiple
    int multiple_center = 11;
    int multiple_sakai = 17;
    int multiple_inverseSakai = 17;
    int multiple_sakaiCorner = 18;
    int multiple_Bros = 15;
    int x_lookup, y_lookup;

    //置いたコマ（next）に対するlookup
    evaluation_putStateEdge = b23m6g26r_lookup_edge[next[nextI][0]][next[nextI][1]];
    evaluation_putStateCenter = b23m6g26r_lookup_center[next[nextI][0]][next[nextI][1]];
    evaluation_putStateSakai = b23m6g26r_lookup_sakai[next[nextI][0]][next[nextI][1]];
    evaluation_putStateInverseSakai = b23m6g26r_inverse_lookup_sakai[next[nextI][0]][next[nextI][1]];
    evaluation_putStateSakaiCorner = b23m6g26r_lookup_sakai_corner[next[nextI][0]][next[nextI][1]];
    evaluation_putStateBros = b23m6g26r_lookup_bros[next[nextI][0]][next[nextI][1]];


    //反転させたコマ（reverceState）に対するlookup.
    for(int i=0; reverceState[i][0]!=-1; i++){
        x_lookup = reverceState[i][0];
        y_lookup = reverceState[i][1];

        evaluation_reverseEdge = evaluation_reverseEdge + b23m6g26r_lookup_edge[x_lookup][y_lookup];
        evaluation_reverseCenter = evaluation_reverseCenter + b23m6g26r_lookup_center[x_lookup][y_lookup];
        evaluation_reverseSakai = evaluation_reverseSakai + b23m6g26r_lookup_sakai[x_lookup][y_lookup];
        evaluation_reverseInverseSakai = evaluation_reverseInverseSakai + b23m6g26r_inverse_lookup_sakai[x_lookup][y_lookup];
        evaluation_reverseSakaiCorner = evaluation_reverseSakaiCorner + b23m6g26r_lookup_sakai_corner[x_lookup][y_lookup];
        evaluation_reverseBros = evaluation_reverseBros + b23m6g26r_lookup_bros[x_lookup][y_lookup];
    }

    //評価値の計算
    evaluation = evaluation + (int)(coef_EvalFunc[4] * (evaluation_putStateEdge*5 + evaluation_reverseEdge) * multiple_edge);  //putStateの影響力の調整として5倍
    evaluation = evaluation + (int)(coef_EvalFunc[5] * (evaluation_putStateCenter*5 + evaluation_reverseCenter) * multiple_center);
    evaluation = evaluation + (int)(coef_EvalFunc[6] * (evaluation_putStateSakai*5 + evaluation_reverseSakai) * multiple_sakai);
    evaluation = evaluation + (int)(coef_EvalFunc[7] * (evaluation_putStateInverseSakai*5 + evaluation_reverseInverseSakai) * multiple_inverseSakai);
    evaluation = evaluation + (int)(coef_EvalFunc[8] * (evaluation_putStateSakaiCorner*5 + evaluation_reverseSakaiCorner) * multiple_sakaiCorner);
    evaluation = evaluation + (int)(coef_EvalFunc[9] * (evaluation_putStateBros*5 + evaluation_reverseBros) * multiple_Bros);

    //-1000~1000になっているかの確認
    EVNew[4] = (evaluation_putStateEdge*5 + evaluation_reverseEdge) * multiple_edge;
    EVNew[5] = (evaluation_putStateCenter*5 + evaluation_reverseCenter) * multiple_center;
    EVNew[6] = (evaluation_putStateSakai*5 + evaluation_reverseSakai) * multiple_sakai;
    EVNew[7] = (evaluation_putStateInverseSakai*5 + evaluation_reverseInverseSakai) * multiple_inverseSakai;
    EVNew[8] = (evaluation_putStateSakaiCorner*5 + evaluation_reverseSakaiCorner) * multiple_sakaiCorner;
    EVNew[9] = (evaluation_putStateBros*5 + evaluation_reverseBros) * multiple_Bros;

    return evaluation;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//盤面へのlookupの値の計算用評価関数
int b23m6g26r_lookup_board(int x, int y, int turn){
    //評価値
    int evaluation=0;
    int evaluation_Edge = 0;
    int evaluation_Center = 0;
    int evaluation_Sakai = 0;
    int evaluation_InverseSakai = 0;
    int evaluation_SakaiCorner = 0;
    int evaluation_Bros = 0;
    //評価値の大きさの調整用
    int multiple_edge = 4;
    int multiple_center = 8;
    int multiple_sakai = 8;
    int multiple_inverseSakai = 7;
    int multiple_sakaiCorner = 8;
    int multiple_Bros = 7;
    
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            if(state[i][j]==enemyTurn){  //相手のコマなのでマイナス
                //lookupを参照
                evaluation_Edge = evaluation_Edge - b23m6g26r_lookup_edge[i][j];
                evaluation_Center = evaluation_Center - b23m6g26r_lookup_center[i][j];
                evaluation_Sakai = evaluation_Sakai - b23m6g26r_lookup_sakai[i][j];
                evaluation_InverseSakai = evaluation_InverseSakai - b23m6g26r_inverse_lookup_sakai[i][j];
                evaluation_SakaiCorner = evaluation_SakaiCorner - b23m6g26r_lookup_sakai_corner[i][j];
                evaluation_Bros = evaluation_Bros - b23m6g26r_lookup_bros[i][j];

            }else if(state[i][j]==myTurn){  //自身のコマなのでプラス
                //lookupを参照
                evaluation_Edge = evaluation_Edge + b23m6g26r_lookup_edge[i][j];
                evaluation_Center = evaluation_Center + b23m6g26r_lookup_center[i][j];
                evaluation_Sakai = evaluation_Sakai + b23m6g26r_lookup_sakai[i][j];
                evaluation_InverseSakai = evaluation_InverseSakai + b23m6g26r_inverse_lookup_sakai[i][j];
                evaluation_SakaiCorner = evaluation_SakaiCorner + b23m6g26r_lookup_sakai_corner[i][j];
                evaluation_Bros = evaluation_Bros + b23m6g26r_lookup_bros[i][j];
                
            }else{  //空きマスなので何もしない
            }
        }
    }

    //評価値の計算
    evaluation = (int)(coef_EvalFunc[10] * (evaluation_Edge * multiple_edge));
    evaluation = evaluation + (int)(coef_EvalFunc[11] * (evaluation_Center * multiple_center));
    evaluation = evaluation + (int)(coef_EvalFunc[12] * (evaluation_Sakai * multiple_sakai));
    evaluation = evaluation + (int)(coef_EvalFunc[13] * (evaluation_InverseSakai * multiple_inverseSakai));
    evaluation = evaluation + (int)(coef_EvalFunc[14] * (evaluation_SakaiCorner * multiple_sakaiCorner));
    evaluation = evaluation + (int)(coef_EvalFunc[15] * (evaluation_Bros * multiple_Bros));
    
    //-1000~1000になっているかの確認
    EVNew[10] = evaluation_Edge * multiple_edge;
    EVNew[11] = evaluation_Center * multiple_center;
    EVNew[12] = evaluation_Sakai * multiple_sakai;
    EVNew[13] = evaluation_InverseSakai * multiple_inverseSakai;
    EVNew[14] = evaluation_SakaiCorner * multiple_sakaiCorner;
    EVNew[15] = evaluation_Bros * multiple_Bros;

    return evaluation;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//中心(4×4)の中にある自身のコマの割合を5割以上に維持する評価関数
int b23m6g26r_rateMore50_center(int x, int y, int turn){
    int evaluation=0;
    int myCount=0;
    int allCount=0;
    float rate_myCount=0;
    int multiple=1000;

    //中心のみを見る. state[][]には0から入っているので2~5
    for(int i=2;i<=5;i++){
        for(int j=2;j<=5;j++){
            if(state[i][j]==2){  //自身のコマ数のカウント
                myCount++;
            }
            if(state[i][j]!=0)  allCount++;  //自身, 相手のコマ数のカウント
        }
    }
    //評価値の計算
    rate_myCount = ((float)myCount / (float)allCount);
    if(rate_myCount>0.5){
        evaluation = 1 * multiple;
    }else{
        evaluation = (int)(rate_myCount*multiple);
    }
    return evaluation;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//取っている角に近いマスほど大きな重みをつける評価関数
int b23m6g26r_weight_ifTakeCorner(int x, int y, int turn, int nextI){
    
    int evaluation =  0;
    int x_tmplookup, y_tmplookup;
    int tmp_lookup[8][8] = {0};
    int multiple = 11;
    //lookup_ifTakeCorner[8][8]を参照
    //tmp_lookup_ifTakeCornerの作成. 角を取っていない象限は0のまま
    if(state[0][0]==myTurn){
        //左上角が自身のコマ
        for(int i=0;i<=3;i++){for(int j=0;j<=3;j++){tmp_lookup[i][j] = lookup_ifTakeCorner[i][j];}}
    }
    if(state[0][7]==myTurn){
        //右上角が自身のコマ
        for(int i=0;i<=3;i++){for(int j=4;j<=7;j++){tmp_lookup[i][j] = lookup_ifTakeCorner[i][j];}}
    }
    if(state[7][0]==myTurn){
        //左下角が自身のコマ
        for(int i=4;i<=7;i++){for(int j=0;j<=3;j++){tmp_lookup[i][j] = lookup_ifTakeCorner[i][j];}}
    }
    if(state[7][7]==myTurn){
        //右下角が自身のコマ
        for(int i=4;i<=7;i++){for(int j=4;j<=7;j++){tmp_lookup[i][j] = lookup_ifTakeCorner[i][j];}}
    }
    
    //置いたコマの評価値. 影響力の調整として5倍
    evaluation = evaluation + (5 * tmp_lookup[next[nextI][0]][next[nextI][1]]);
    //反転させたコマの評価値
    for(int i=0; reverceState[i][0]!=-1; i++){
        x_tmplookup = reverceState[i][0];
        y_tmplookup = reverceState[i][1];
        evaluation = evaluation + tmp_lookup[x_tmplookup][y_tmplookup];
    }
    evaluation = evaluation * multiple;
    return evaluation;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//コマの数において自身/相手の割合が高い象限を優先する評価関数
int b23m6g26r_rateMore_MyPerEnemy_quadrant(int x, int y, int turn, int nextI, int state_save[8][8]){
    
    int evaluation = 0;
    int multiple = 17;
    int count_MyAndEnemy[4][2] = {0};  //4象限で自身、相手のコマ数を保存用変数
    int rate_MyPerEnemy[4][2] = {0};  //[][0]に割合、[][1]に割合が他の象限以上の際の回数
    int cnt_orMore=0;  //割合が他の象限以上の回数
    int tmp_lookup[8][8] = {0};
    int x_tmplookup, y_tmplookup;

    for(int i=0;i<=3;i++){  //左上
        for(int j=0;j<=3;j++){
            if(state_save[i][j]==myTurn){  //自身のコマ
                count_MyAndEnemy[0][0]++;
            }else if(state_save[i][j]==enemyTurn){  //相手のコマ
                count_MyAndEnemy[0][1]++;
            }
        }
    }
    for(int i=0;i<=3;i++){  //右上
        for(int j=4;j<=7;j++){
            if(state_save[i][j]==myTurn){  //自身のコマ
                count_MyAndEnemy[1][0]++;
            }else if(state_save[i][j]==enemyTurn){  //相手のコマ
                count_MyAndEnemy[1][1]++;
            }
        }
    }
    for(int i=4;i<=7;i++){  //左下
        for(int j=0;j<=3;j++){
            if(state_save[i][j]==myTurn){  //自身のコマ
                count_MyAndEnemy[2][0]++;
            }else if(state_save[i][j]==enemyTurn){  //相手のコマ
                count_MyAndEnemy[2][1]++;
            }
        }
    }
    for(int i=4;i<=7;i++){  //右下
        for(int j=4;j<=7;j++){
            if(state_save[i][j]==myTurn){  //自身のコマ
                count_MyAndEnemy[4][0]++;
            }else if(state_save[i][j]==enemyTurn){  //相手のコマ
                count_MyAndEnemy[4][1]++;
            }
        }
    }
    
    //各象限の自身のコマ数/相手のコマ数. 0で割ることとならないように1を加算
    count_MyAndEnemy[0][0]++;count_MyAndEnemy[0][1]++;
    count_MyAndEnemy[1][0]++;count_MyAndEnemy[1][1]++;
    count_MyAndEnemy[2][0]++;count_MyAndEnemy[2][1]++;
    count_MyAndEnemy[3][0]++;count_MyAndEnemy[3][1]++;
    rate_MyPerEnemy[0][0] = count_MyAndEnemy[0][0]/count_MyAndEnemy[0][1];  //左上の象限の割合
    rate_MyPerEnemy[1][0] = count_MyAndEnemy[1][0]/count_MyAndEnemy[1][1];  //右上の象限の割合
    rate_MyPerEnemy[2][0] = count_MyAndEnemy[2][0]/count_MyAndEnemy[2][1];  //左下の象限の割合
    rate_MyPerEnemy[3][0] = count_MyAndEnemy[3][0]/count_MyAndEnemy[3][1];  //右下の象限の割合

    //任意の象限と4象限で大小関係の比較, カウント
    for(int i=0;i<4;i++){
        if(rate_MyPerEnemy[i][0] >= rate_MyPerEnemy[0][0]) cnt_orMore++;
        if(rate_MyPerEnemy[i][0] >= rate_MyPerEnemy[1][0]) cnt_orMore++;
        if(rate_MyPerEnemy[i][0] >= rate_MyPerEnemy[2][0]) cnt_orMore++;
        if(rate_MyPerEnemy[i][0] >= rate_MyPerEnemy[3][0]) cnt_orMore++;
        cnt_orMore--;  //任意の象限同士で比較してカウントしているので-1
        rate_MyPerEnemy[i][1] = cnt_orMore;
        cnt_orMore = 0;
    }
    //lookupへの重みづけ
    for(int i=0;i<=3;i++){  //左上
        for(int j=0;j<=3;j++){
            tmp_lookup[i][j] = rate_MyPerEnemy[0][1];
        }
    }
    for(int i=0;i<=3;i++){  //右上
        for(int j=4;j<=7;j++){
            tmp_lookup[i][j] = rate_MyPerEnemy[1][1];
        }
    }
    for(int i=4;i<=7;i++){  //左下
        for(int j=0;j<=3;j++){
            tmp_lookup[i][j] = rate_MyPerEnemy[2][1];
        }
    }
    for(int i=4;i<=7;i++){  //右下
        for(int j=4;j<=7;j++){
            tmp_lookup[i][j] = rate_MyPerEnemy[3][1];
        }
    }

    //置いたコマの評価値. 影響力の調整として5倍
    evaluation = evaluation + (5 * tmp_lookup[next[nextI][0]][next[nextI][1]]);
    //反転させたコマの評価値
    for(int i=0; reverceState[i][0]!=-1; i++){
        x_tmplookup = reverceState[i][0];
        y_tmplookup = reverceState[i][1];
        evaluation = evaluation + tmp_lookup[x_tmplookup][y_tmplookup];
    }
    evaluation = evaluation * multiple;
    
    return evaluation;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//敵（自身なら相手、相手なら自身）のマスが周囲に多い場所を避ける評価関数. 端は重み強め.
int b23m6g26r_leaveOne(int x, int y, int turn, int nextI){
    int x_tmp = next[nextI][0];  //x
    int y_tmp = next[nextI][1];  //y
    int evaluation = 0;
    int multiple = 1000;  //evaluationの最大値が1なので1000倍
    int coef_edge = 2; //nextが端だった際の係数
    int cnt_notNextState = 0;  //次に手を打つ人ではない人のコマ.
    int notNextTurn;  //次に手を打つ人ではない人. 敵（自身なら相手、相手なら自身）

    if(turn==1){
        notNextTurn=2;
    }else{
        notNextTurn=1;
    }

    //角ならevaluation=2 or 角を優先する評価値は用意しない
    if((x_tmp==0 && y_tmp==0)||(x_tmp==0 && y_tmp==7)||(x_tmp==7 && y_tmp==0)||(x_tmp==7 && y_tmp==7)){        
        return evaluation;  //0. 角は反転させられないので隣のコマを気にする必要はない
    }else{
        //角以外の端
        if(x_tmp==0){  //上端
            if(state[x_tmp][y_tmp-1]==notNextTurn){          cnt_notNextState++;
            }else if(state[x_tmp][y_tmp+1]==notNextTurn){    cnt_notNextState++;
            }else if(state[x_tmp+1][y_tmp-1]==notNextTurn){  cnt_notNextState++;
            }else if(state[x_tmp+1][y_tmp]==notNextTurn){    cnt_notNextState++;
            }else if(state[x_tmp+1][y_tmp+1]==notNextTurn){  cnt_notNextState++;
            }
            evaluation = (int)(1.0/(cnt_notNextState+1.0) * multiple);
            return evaluation;

        }else if(y_tmp==0){  //左端
            if(state[x_tmp-1][y_tmp]==notNextTurn){          cnt_notNextState++;
            }else if(state[x_tmp-1][y_tmp+1]==notNextTurn){  cnt_notNextState++;
            }else if(state[x_tmp][y_tmp+1]==notNextTurn){    cnt_notNextState++;
            }else if(state[x_tmp+1][y_tmp]==notNextTurn){    cnt_notNextState++;
            }else if(state[x_tmp+1][y_tmp+1]==notNextTurn){  cnt_notNextState++;
            }
            evaluation = (int)(1.0/(cnt_notNextState+1.0) * multiple);
            return evaluation;
        
        }else if(y_tmp==7){  //右端
            if(state[x_tmp-1][y_tmp-1]==notNextTurn){        cnt_notNextState++;
            }else if(state[x_tmp-1][y_tmp]==notNextTurn){    cnt_notNextState++;
            }else if(state[x_tmp][y_tmp-1]==notNextTurn){    cnt_notNextState++;
            }else if(state[x_tmp+1][y_tmp-1]==notNextTurn){  cnt_notNextState++;
            }else if(state[x_tmp+1][y_tmp]==notNextTurn){    cnt_notNextState++;
            }
            evaluation = (int)(1.0/(cnt_notNextState+1.0) * multiple);
            return evaluation;

        }else if(x_tmp==7){  //下端
            if(state[x_tmp-1][y_tmp-1]==notNextTurn){        cnt_notNextState++;
            }else if(state[x_tmp-1][y_tmp]==notNextTurn){    cnt_notNextState++;
            }else if(state[x_tmp-1][y_tmp+1]==notNextTurn){  cnt_notNextState++;
            }else if(state[x_tmp][y_tmp-1]==notNextTurn){    cnt_notNextState++;
            }else if(state[x_tmp][y_tmp+1]==notNextTurn){    cnt_notNextState++;
            }
            evaluation = (int)(1.0/(cnt_notNextState+1.0) * multiple);
            return evaluation;
        }
    
        //nextが角、端以外の場所
        if(state[x_tmp-1][y_tmp-1]==notNextTurn){        cnt_notNextState++;
        }else if(state[x_tmp-1][y_tmp]==notNextTurn){    cnt_notNextState++;
        }else if(state[x_tmp-1][y_tmp+1]==notNextTurn){  cnt_notNextState++;
        }else if(state[x_tmp][y_tmp-1]==notNextTurn){    cnt_notNextState++;
        }else if(state[x_tmp][y_tmp+1]==notNextTurn){    cnt_notNextState++;
        }else if(state[x_tmp+1][y_tmp-1]==notNextTurn){  cnt_notNextState++;
        }else if(state[x_tmp+1][y_tmp]==notNextTurn){    cnt_notNextState++;
        }else if(state[x_tmp+1][y_tmp+1]==notNextTurn){  cnt_notNextState++;
        }
        evaluation = (int)(1.0/(cnt_notNextState+1.0) * multiple);
        return evaluation;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//周りのマスに空きが多いほど高評価、空きが少ないほど高評価にする評価関数. mode=0:空きが多いほど, mode=1：空きが少ないほど, mode=2:両方
int b23m6g26r_MoreAndLess_spaceAround(int x, int y, int turn, int nextI, int mode){
    int x_tmp = next[nextI][0];  //x
    int y_tmp = next[nextI][1];  //y
    int cnt_aroundState = 0;
    int evaluation = 0;
    int multiple_more = 1000;  //evaluationの最大値が1なので1000倍
    int multiple_less = 1000;  //evaluationの最大値が1なので1000倍
    int coef_edge = 2; //nextが端だった際の係数
    int flag_edge = 0;
    float denominator = 8.0;  //分母. 端なら5, それ以外なら8
    
    //角ならevaluation=2 or 角を優先する評価値は用意しない
    if((x_tmp==0 && y_tmp==0)||(x_tmp==0 && y_tmp==7)||(x_tmp==7 && y_tmp==0)||(x_tmp==7 && y_tmp==7)){        
        return evaluation;  //0. 角は反転させられないので隣のコマを気にする必要はない
    }else{  //角以外の端
        if(x_tmp==0){  //上端
            if(state[x_tmp][y_tmp-1]!=0){          cnt_aroundState++;
            }else if(state[x_tmp][y_tmp+1]!=0){    cnt_aroundState++;
            }else if(state[x_tmp+1][y_tmp-1]!=0){  cnt_aroundState++;
            }else if(state[x_tmp+1][y_tmp]!=0){    cnt_aroundState++;
            }else if(state[x_tmp+1][y_tmp+1]!=0){  cnt_aroundState++;
            }
            denominator = 5.0;

        }else if(y_tmp==0){  //左端
            if(state[x_tmp-1][y_tmp]!=0){          cnt_aroundState++;
            }else if(state[x_tmp-1][y_tmp+1]!=0){  cnt_aroundState++;
            }else if(state[x_tmp][y_tmp+1]!=0){    cnt_aroundState++;
            }else if(state[x_tmp+1][y_tmp]!=0){    cnt_aroundState++;
            }else if(state[x_tmp+1][y_tmp+1]!=0){  cnt_aroundState++;
            }
            denominator = 5.0;

        }else if(y_tmp==7){  //右端
            if(state[x_tmp-1][y_tmp-1]!=0){        cnt_aroundState++;
            }else if(state[x_tmp-1][y_tmp]!=0){    cnt_aroundState++;
            }else if(state[x_tmp][y_tmp-1]!=0){    cnt_aroundState++;
            }else if(state[x_tmp+1][y_tmp-1]!=0){  cnt_aroundState++;
            }else if(state[x_tmp+1][y_tmp]!=0){    cnt_aroundState++;
            }
            denominator = 5.0;

        }else if(x_tmp==7){  //下端
            if(state[x_tmp-1][y_tmp-1]!=0){        cnt_aroundState++;
            }else if(state[x_tmp-1][y_tmp]!=0){    cnt_aroundState++;
            }else if(state[x_tmp-1][y_tmp+1]!=0){  cnt_aroundState++;
            }else if(state[x_tmp][y_tmp-1]!=0){    cnt_aroundState++;
            }else if(state[x_tmp][y_tmp+1]!=0){    cnt_aroundState++;
            }
            denominator = 5.0;

        }else{  //nextが端ではなかったら計算
            //nextが角、端以外の場所
            if(state[x_tmp-1][y_tmp-1]!=0){        cnt_aroundState++;
            }else if(state[x_tmp-1][y_tmp]!=0){    cnt_aroundState++;
            }else if(state[x_tmp-1][y_tmp+1]!=0){  cnt_aroundState++;
            }else if(state[x_tmp][y_tmp-1]!=0){    cnt_aroundState++;
            }else if(state[x_tmp][y_tmp+1]!=0){    cnt_aroundState++;
            }else if(state[x_tmp+1][y_tmp-1]!=0){  cnt_aroundState++;
            }else if(state[x_tmp+1][y_tmp]!=0){    cnt_aroundState++;
            }else if(state[x_tmp+1][y_tmp+1]!=0){  cnt_aroundState++;
            }
        }

        if(mode==0){  //more
            evaluation = (int)((cnt_aroundState/denominator) * multiple_more * coef_EvalFunc[20]);
        }else if(mode==1){  //less
            evaluation = (int)(1.0/(cnt_aroundState) * multiple_less * coef_EvalFunc[21]);
        }else{  //more and less
            evaluation = (int)((cnt_aroundState/denominator) * multiple_more * coef_EvalFunc[20]);
            evaluation = evaluation + (int)(1.0/(cnt_aroundState) * multiple_less * coef_EvalFunc[21]);

            EVNew[20] = (int)((cnt_aroundState/denominator) * multiple_more);
            EVNew[21] = (int)(1.0/(cnt_aroundState) * multiple_less);
        }
        return evaluation;
    }
    
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//手数を多いほど高評価、少ないほど高評価にする評価関数. mode=0:多く, mode=1:少なく, mode=2:両方
int b23m6g26r_MoreAndLess_nextNum(int x, int y, int turn, int nextI, int mode){
    int evaluation = 0;
    int evaluation_more = 0;
    int evaluation_less = 0;
    int multiple = 1000;  //evaluationの最大値が1なので1000倍
    int cnt_nextNum=0;
    while( next[cnt_nextNum][0]!=-1 ){
        cnt_nextNum++;
    }
    if(cnt_nextNum > 10){
        cnt_nextNum = 10;  //丸め込み
    }
    if(mode==0){  //more
        evaluation = (int)((cnt_nextNum/10.0) * multiple * coef_EvalFunc[22]);
    }else if(mode==1){  //less
        evaluation = (int)((1.0/cnt_nextNum) * multiple * coef_EvalFunc[23]);
    }else{  //more and less
        evaluation_more = (int)(cnt_nextNum/10.0 * multiple * coef_EvalFunc[22]);  //multipleとcoef_EvalFunc[]をかけてからintに変換しないといけない
        evaluation_less = (int)(1.0/cnt_nextNum * multiple * coef_EvalFunc[23]);
        evaluation = evaluation_more + evaluation_less;

        EVNew[22] = (int)(cnt_nextNum/10.0 * multiple);
        EVNew[23] = (int)(1.0/cnt_nextNum * multiple);
    }
    return evaluation;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//盤面全体で自身のコマ:相手のコマ=1:1を維持する評価関数
int b23m6g26r_rate50_board(int x, int y, int turn){
    int evaluation=0;
    int myCount=0, enemyCount=0;
    float myCount_float=0.0, enemyCount_float=0.0;
    int multiple=1000;

    //自身と相手のコマのカウント
    if(myTurn==1){
        checkState(&myCount, &enemyCount);
    }else{
        checkState(&enemyCount, &myCount);
    }
    myCount_float = (float)myCount;
    enemyCount_float = (float)enemyCount;
    

    //rate==1から離れるほど低評価
    if(myCount < enemyCount){
        evaluation = (int)(myCount_float / enemyCount_float * multiple);
    }else{
        evaluation = (int)(enemyCount_float / myCount_float * multiple);
    }
    return evaluation;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//縦横斜めの被り多いほど高評価、少ないほど高評価にする評価関数. mode=0:多く, mode=1:少なく, mode=2:両方
int b23m6g26r_overlap_VertivalHorizontalDiagonal(int x, int y, int turn, int nextI, int state_save[8][8], int mode){
    int x_tmp;  //x
    int y_tmp;  //y
    float cnt_myState = 0;  //各nextごと. 方向ごとではなく.
    float cnt_allState = 0;  //各nextごと. 方向ごとではなく.
    int evaluation = 0;
    int evaluation_less = 0;
    int evaluation_more = 0;
    int multiple = 1000;  //evaluationの最大値が1なので1000倍
    int direction = 0;  //0:左上, 1:真上, 2:右上, 3:左横, 4:右横, 5:左下, 6:真下, 7:右下
    int lenExplore;
    int x_dire, y_dire;  //directionに対応したx, y

    //nextのx, yの格納
    for(direction=0; direction<=7; direction++){
        //x, yの初期化
        x_tmp = next[nextI][0];
        y_tmp = next[nextI][1];
        lenExplore = 0;

        //探索する方向を決定. 左上ならx_dire=-1,y_dire=-1
        switch(direction){
            case 0: x_dire=-1; y_dire=-1; break;
            case 1: x_dire=-1; y_dire= 0; break;
            case 2: x_dire=-1; y_dire= 1; break;
            case 3: x_dire= 0; y_dire=-1; break;
            case 4: x_dire= 0; y_dire= 1; break;
            case 5: x_dire= 1; y_dire=-1; break;
            case 6: x_dire= 1; y_dire= 0; break;
            case 7: x_dire= 1; y_dire= 1; break;
        }

        for(int i=0;i<=7;i++){
            //1マス遠くへ探索. 初期はnextの隣のマス. x_dire, y_direに0*lenExploreを使うことで調整
            lenExplore++;
            x_tmp = x_dire * lenExplore;
            y_tmp = y_dire * lenExplore;

            //判定
            if( 0>x_tmp || x_tmp>7 || 0>y_tmp || y_tmp>7)  break;  //盤面外なのでbreak
            //nextを置く前の盤面に対して判定
            if(state_save[x_tmp][y_tmp]==myTurn)  cnt_myState = cnt_myState + 1.0;
            if(state_save[x_tmp][y_tmp]!=0)  cnt_allState = cnt_allState + 1.0;  //自身か相手のコマなら加算

        }
    }
    //評価値計算
    if(mode==0){  //less
        evaluation = (int)((cnt_allState - cnt_myState) / cnt_allState * multiple * coef_EvalFunc[25]);  //少ないほど高評価
    }else if(mode==1){  //more
        evaluation = (int)(cnt_myState / cnt_allState * multiple * coef_EvalFunc[26]);  //多いほど高評価
    }else{
        evaluation_less = (int)((cnt_allState - cnt_myState) / cnt_allState * multiple * coef_EvalFunc[25]);  //少ないほど高評価
        evaluation_more = (int)(cnt_myState / cnt_allState * multiple * coef_EvalFunc[26]);  //多いほど高評価
        evaluation = evaluation_less + evaluation_more;
        
        EVNew[25] = (int)((cnt_allState - cnt_myState) / cnt_allState * multiple);
        EVNew[26] = (int)(cnt_myState / cnt_allState * multiple);
    }

    return evaluation;
    
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//極端な個数とる手、中途半端な個数とる手を優先する評価関数
int b23m6g26r_take_ExtremeOrHalfWay(int x, int y, int turn, int myCount_old, int myCount_new, int mode){    
    int newMyNum = 0;  //nextを置くことにより新しく自身のコマになるコマの数
    int reverseNum = 0;
    int evaluation = 0;
    int evaluation_extreme = 0;
    int evaluation_halfway = 0;
    float takeMore, takeLess; 
    int multiple = 900;  //少し1を超えるため
    int maxReverse = 12, minReverse = -12;
    int CountDifference=0;

    CountDifference = myCount_new - myCount_old;

    //丸め込み
    if(CountDifference >= maxReverse || CountDifference <= minReverse){
        evaluation = 1.1 * multiple;
    }else{
        //正の値に調整
        CountDifference = CountDifference + 13;
        maxReverse = maxReverse + 13;
        minReverse = minReverse + 13;

        //後半に取れるコマ数として3個が出ても6個以上を優先させるような計算式
        takeLess = powf((0.9/CountDifference), 2.0)*1.3;
        takeMore = powf((CountDifference/25.0), 12.0)*1.3;

        if(mode==0){  //extreme
            evaluation = (int)(takeLess + takeMore * multiple * coef_EvalFunc[27]);

        }else if(mode==1){  //halfway
            evaluation = (int)(-1 * (takeLess + takeMore) + 1.1 * multiple * coef_EvalFunc[28]);  //上向きの放物線. 正の値になるようにずらす

        }else{  //extreme and halfway
            evaluation_extreme = (int)(takeLess + takeMore * multiple * coef_EvalFunc[27]);
            evaluation_halfway = (int)(-1 * (takeLess + takeMore) + 1.1 * multiple * coef_EvalFunc[28]);
            evaluation = evaluation_extreme + evaluation_halfway;

            EVNew[27] = (int)(takeLess + takeMore * multiple);
            EVNew[28] = (int)(-1 * (takeLess + takeMore) + 1.1 * multiple);
        }
    }
    return evaluation;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//最近置いた、反転させたコマを反転させられないようにする評価関数
int b23m6g26r_isReversed_newMyState(int x, int y, int turn, int nextI, int lookup_RevdNewMy[8][8]){    
    int weight = 0;
    int x_tmp, y_tmp;
    int evaluation = 0;
    int multiple = 3;

    if(turn == myTurn){ 
        //自身のturnならlookupの重みの更新
        for(int i=0;i<=7;i++){
            for(int j=0;j<=7;j++){
                //weight = lookup[i][j];
                weight = lookup_RevdNewMy[i][j];  //各マスの重みを確認
                switch(weight){  //重みに対応して重みの更新
                    case  0: 
                    case  1: lookup_RevdNewMy[i][j]=0;
                    case  2: lookup_RevdNewMy[i][j]=1;
                    case  3: lookup_RevdNewMy[i][j]=2;
                    case  5: lookup_RevdNewMy[i][j]=1;
                    case 10: lookup_RevdNewMy[i][j]=5;
                }
            }
        }
        //nextとreverceStateの重みを上書き
        x_tmp = next[nextI][0];
        y_tmp = next[nextI][1];
        lookup_RevdNewMy[x_tmp][y_tmp] = 10;  //next
        for(int i=0;i<60;i++){
            if(reverceState[i][0]==-1)  break;
            x_tmp = reverceState[i][0];
            y_tmp = reverceState[i][1];
            lookup_RevdNewMy[x_tmp][y_tmp] = 3;  //reverceState
        }
        return 0;

    }else{
        //相手のturnならlookupから評価値の計算
        for(int i=0;i<60;i++){
            if(reverceState[i][0]==-1)  break;
            x_tmp = reverceState[i][0];
            y_tmp = reverceState[i][1];
            evaluation = evaluation + lookup_RevdNewMy[x_tmp][y_tmp];
        }
        evaluation = evaluation * (-1) * multiple;
        return evaluation;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
//先読みalpha-beta法
//最初のab_max()
int b23m6g26r_ab_max_FirstTime(int *x, int *y, int turn, int limit, int alpha, int beta)
{
    cnt_explore[index_cnt_explore]++;  //探索回数のカウント

    //評価値、最も良い評価値の変数の初期化. 
    int v=0, v_save=-100000;
    
    //nextの初期化
    for(int i=0;i<=59;i++)
        for(int j=0;j<=1;j++)
            next[i][j]=-1;
    //nextの生成.
    makeNextList( turn );
    
    //(1,9)のように9が入る対策として一旦入れておく
    *x=next[0][0];
    *y=next[0][1];

    //局面のローカル保存
    int state_save[8][8];
    int next_save[60][2];
    b23m6g26r_state_SaveOrLoad_arg1Toarg2(state, state_save);  //int state_save = state : save
    b23m6g26r_next_SaveOrLoad_arg1Toarg2(next, next_save);  //int next_save = next : save

    //自分と相手のコマ数のカウント
    int myCount=0, enemyCount=0;
    if(myTurn==1){
        checkState(&myCount, &enemyCount);
    }else{
        checkState(&enemyCount, &myCount);
    }
    
    //評価関数用にFirstTimeでのコマ数を保存
    StateNum_FirstTime = myCount;

    //コマがない際の評価値
    int EvalVal_noCount = 0;
    if(enemyCount <= 0){
        EvalVal_noCount = EvalVal_noCount + 10000;
        printf("turn:%d enemyCount<=0\n", turn);  //優先する, ローカル評価関数
        //exit(1);  //確認
        return EvalVal_noCount;

    }else if(myCount <= 0){
        EvalVal_noCount = EvalVal_noCount - 10000;
        printf("turn:%d myCount<=0\n", turn);  //避ける
        //exit(1);
        return EvalVal_noCount;
    }

    //次の手のパスの確認. FirstTimeでパスなら先読みはしても意味がない
    int pass_Consecutive_Max2 = 0;  //2回連続でパスをしたらその際のコマ数を判定して、勝てるなら優先、負けるなら避けるようにreturnする
    int flag_noMyNext = 0;  //パスをするか否かのフラグ
    if(next[0][0]==-1){
        flag_noMyNext = 1;  //パスをするので1    
    }

    //評価値に関する変数
    //int EvalVal_XX[20] = {0};  //Corner, Pass等の評価値を運ぶリスト？
    int EvalVal_Corner = 0;  //角の評価値
    int EvalVal_Pass = 0;   //パスの評価値
    int EvalVal_RevdNewMy = 0;  //EvalVal_isReversedNewMyState;
    int lookup_RevdNewMy[8][8] = {0};  //lookup_isReversedNew[8][8]

    //評価関数. パスの場合は入らない
    if((limit == 0 || myCount+enemyCount >= 63 || enemyCount<=1)  && flag_noMyNext==0){        
        if(mode == 0){
            return b23m6g26r_RandomAgent_FirstTime(x, y, turn, 5);  //先読みの確認, 5は適当
        
        }else{
            int ReturnEvaluation = 0;
            fromAbmaxFirstTime = 1;  //評価関数内でのx, yの上書き
            ReturnEvaluation =  b23m6g26r_EvaluationAndPolynomial(x, y, turn, next_save, state_save, EvalVal_Corner, EvalVal_Pass, EvalVal_RevdNewMy, lookup_RevdNewMy);
            fromAbmaxFirstTime = 0;  //権限を取り下げる
            return ReturnEvaluation;
        }
    }

    //エラー時の強制終了
    //limitを超えて先読みをしている
    if(limit < 0){
        //printf("limit : %d\n", limit);
        //exit(1);
        return 0;
    }
    //終局ならエラーを出してもらう
    if(checkFinish()==1){
        //printf("error : checkFinish in ab_max_FirstTime()\n");
        //exit(1);
        return 0;
    }
    //nextに(1,9)はないのに置くので一旦next内の確認
    for(int i=0; next[i][0]!=-1; i++){
        if(next[i][0]<0 || next[i][0]>7 || next[i][1]<0 || next[i][1]>7){
            //printf("ab_max_FirstTime() next error : next[%d][0]=%d, next[%d][1]=%d\n", i, next[i][0], i, next[i][1]);
            //exit(1);
            return 0;
        }
    }

    //先読み. FirstTimeでパスする場合は先読みはしない
    for(int i=0; next[i][0]!=-1; i++){
        
        //打つ手(next)が角か否か
        if(next[i][0]==0 && next[i][1]==0 || next[i][0]==0 && next[i][1]==7 || next[i][0]==7 && next[i][1]==0 || next[i][0]==7 && next[i][1]==7){
            EvalVal_Corner = EvalVal_Corner + 2000*(limit+1);
        }

        // 反転させるコマのリストを初期化
        for(int i=0;i<=59;i++){
            reverceState[i][0]=-1;
            reverceState[i][1]=-1;
        }
        reverceStateNum=0;  //反転させるコマの数を初期化

        //局面に子接点の手を打つ
        b23m6g26r_EvalReverce(next[i][0], next[i][1], turn );  //反転させるコマもカウントする

        //最近取ったコマを取られないようにする評価関数
        EvalVal_RevdNewMy = b23m6g26r_isReversed_newMyState(*x, *y, turn, i, lookup_RevdNewMy);
        
        //次の相手の手
        v = b23m6g26r_ab_min(*x, *y, turn, limit-1, alpha, beta, pass_Consecutive_Max2, EvalVal_Corner, EvalVal_Pass, EvalVal_RevdNewMy, lookup_RevdNewMy);    //printf("ab_max_FirstTime() after ab_min() v : v=%d, v_save=%d\n", v, v_save);

        //局面をもとに戻す. ローカル保存をもとにグローバルstateをもとに戻す
        b23m6g26r_state_SaveOrLoad_arg1Toarg2(state_save, state);  //state <- state_save : load
        b23m6g26r_next_SaveOrLoad_arg1Toarg2(next_save, next);  //next <- next_save : load
        
        //x, yのランダム選択での更新. >なら初期化, ==なら追加保存, <なら無視
        if(v_save < v){
            //vの更新
            v_save = v;

            //x, yの更新.
            *x = next[i][0];
            *y = next[i][1];
            
            //v_List(ランダム選択用リスト), len_v_Listの初期化
            len_v_List = 1;
            for(int i=0; i<60; i++){
                    v_List[i][0] = -1;  //xの初期化
                    v_List[i][1] = -1;  //yの初期化
            }
            //x, yのランダム選択用の更新
            v_List[len_v_List-1][0] = next[i][0];  //lenは1から, 要素番号は0から始めるので-1
            v_List[len_v_List-1][1] = next[i][1];

        }else if(v_save == v){
            len_v_List++;
            //x, yのランダム選択用の保存
            v_List[len_v_List-1][0] = next[i][0];
            v_List[len_v_List-1][1] = next[i][1];
        }

        //alpha-beta法
        if(v > alpha){
            //alphaの更新
            alpha = v;

            //カット
            if(alpha >= beta){
                return beta;
            }
        }   
    }
    return alpha;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//自身の手
int b23m6g26r_ab_max(int x, int y, int turn, int limit, int alpha, int beta, int pass_Consecutive_Max2, int EvalVal_Corner, int EvalVal_Pass, int EvalVal_RevdNewMy, int lookup_RevdNewMy[8][8])
{
    //printf("in ab_max()\n");
    cnt_explore[index_cnt_explore]++;  //探索回数のカウント

    int v;
    if(turn==1)	turn=2;
            else turn=1;
    
    //nextの初期化
    for(int i=0;i<=59;i++)
        for(int j=0;j<=1;j++)
            next[i][j]=-1;

    //nextの生成
    makeNextList( myTurn );

    //局面のローカル保存
    int state_save[8][8];
    int next_save[60][2];
    b23m6g26r_state_SaveOrLoad_arg1Toarg2(state, state_save);  //int state_save <- state : save
    b23m6g26r_next_SaveOrLoad_arg1Toarg2(next, next_save);  //int next_save <- next : save

    int myCount=0, enemyCount=0;
    if(myTurn==1){
        checkState(&myCount, &enemyCount);
    }else{
        checkState(&enemyCount, &myCount);
    }


    //評価関数と多項式の計算
    //パス、コマがない、角を取れる・取られるとかを1つの変数・固定値として渡してもいいかも    //aCount==相手, bCount==自身
    //コマがない
    int EvalVal_noCount = 0;
    if(enemyCount <= 0){
        EvalVal_noCount = EvalVal_noCount + 10000;
        printf("turn:%d enemyCount<=0\n", turn);  //優先する, ローカル評価関数
        //exit(1);
        return EvalVal_noCount;

    }else if(myCount <= 0){
        EvalVal_noCount = EvalVal_noCount - 10000;
        printf("turn:%d myCount<=0\n", turn);  //避ける
        //exit(1);
        return EvalVal_noCount;
    }

    //数手先でパスをせざる負えないパターンがあることを確認しているだけ。    //なのでexit(1)はいらないはず。単純に評価値を下げるだけ。
    //パスの確認
    int flag_noMyNext = 0;
    if(next[0][0]==-1){    
        //まだ評価値は導入できていない//パス. 避ける
        int flag_measure = 1;  //パスの際の対策の選択
        //printf("ab_max : turn:%d pass\n", turn);
        //for(int passNumCount=0; passNumCount<20; passNumCount++) printf("pass");    
        //printf("\n");
        //---------------------------------------------------------------------
        //対策1 : Nextがないなら次の相手の手を評価する----------------------------
        if(flag_measure==1){
            flag_noMyNext = 1;  //パスをするので1
            pass_Consecutive_Max2++;  //カウント
        }
        //---------------------------------------------------------------------
        //対策2 : makeNextList()を再度行う---------------------------------------
        else if(flag_measure==2){
            //パスをしてしまうというか、nextがあるのにないと言っているのでnextやmakeNextList()の問題かも
            for(int i=0; next[i][0]!=-1; i++){
                //IsNotInNext(x, y);  //x,yがnextにあるか
                //printf("next[%d][0]=%d, next[%d][1]=%d\n", i, next[i][0], i, next[i][1]);  //nextの中
            }
            //printf("\nab_max() Re makeNextList()\n");
            makeNextList(turn);  //nextの更新
            for(int i=0; next[i][0]!=-1; i++){
                //IsNotInNext(x, y);  //x,yがnextにあるか
                //printf("next[%d][0]=%d, next[%d][1]=%d\n", i, next[i][0], i, next[i][1]);  //nextの中
            }
        }
        //---------------------------------------------------------------------
    }

    //お互いがパスなので終局になる
    if(pass_Consecutive_Max2 > 1){
        //printf("ab_max : pass_Consecutive_Max2 > 1\n");
        //自身と相手のコマ数を数える。自身の方が多ければ優先。少なければ避ける.
        if(myCount > enemyCount){
            //printf("bCount > aCount\n");  //優先する
            //printf("\nab_min : turn:%d pass\n", turn);  //パス
            //for(int passNumCount=0; passNumCount<20; passNumCount++) printf("pass");    
            //printf("\n");
            return 10000;
        }else{
            //printf("bCount <= aCount\n");  //避ける
            //printf("\nab_min : turn:%d pass\n", turn);  //パス
            //for(int passNumCount=0; passNumCount<20; passNumCount++) printf("pass");    
            //printf("\n");
            return -10000;
        }
    }
    
    //評価関数.
    if((limit == 0 || myCount+enemyCount >= 63 || enemyCount<=1)  && flag_noMyNext==0){  //終局判定. 1マスは空きがある、自身のコマが盤面に1個はある
        if(mode == 0){
            return b23m6g26r_RandomAgent_FirstTime(&x, &y, turn, 5);  //先読みの確認
        
        }else{
            int ReturnEvaluation = 0;
            //next_save, state_save, 評価値を渡して関数内で利用
            ReturnEvaluation = b23m6g26r_EvaluationAndPolynomial(&x, &y, turn, next_save, state_save, EvalVal_Corner, EvalVal_Pass, EvalVal_RevdNewMy, lookup_RevdNewMy);
            return ReturnEvaluation;
        }
    }

    //エラー時の強制終了
    //終局ならエラーを出してもらう
    if(checkFinish()==1){   
        //printf("error? : chechFinish in ab_max()\n");
        //printf("limit=%d\n", limit);
        //printf("enemyCount=%d, myCount=%d\n", enemyCount, myCount);
        //showState();
        //exit(1);
        return 0;
    }
    //limitの超過
    if(limit < 0){
        //printf("limit : %d\n", limit);
        //exit(1);
        return 0;
    }
    //nextに(1,9)に置くことがあるので一旦next内の確認
    for(int i=0; next[i][0]!=-1; i++){
        if(next[i][0]<0 || next[i][0]>7 || next[i][1]<0 || next[i][1]>7){
            //printf("ab_max()) next error : next[%d][0]=%d, next[%d][1]=%d\n", i, next[i][0], i, next[i][1]);
            //exit(1);
            return 0;
        }
    }

    //パスをするときの先読み. nextがないため角の判定はいらない
    if(flag_noMyNext==1){
        limit++;  //パスの分のlimit-1を戻す
        
        //パスの評価値の計算. 自身がパスをするのでマイナス
        EvalVal_Pass = EvalVal_Pass - 1000*(limit+1);

        v = b23m6g26r_ab_min(x, y, turn, limit-1, alpha, beta, pass_Consecutive_Max2, EvalVal_Corner, EvalVal_Pass, EvalVal_RevdNewMy, lookup_RevdNewMy);
        
        if(v > alpha){
            alpha = v;
            if(alpha >= beta){
                return beta;  //カット
            }
        }
        return alpha;
    }

    //パスをせず、次の手を打つときの先読み
    for(int i=0; next[i][0]!=-1; i++){
        pass_Consecutive_Max2 = 0;  //パスをしなかったのでリセット

        //打つ手(next)が角か否か
        if(next[i][0]==0 && next[i][1]==0 || next[i][0]==0 && next[i][1]==7 || next[i][0]==7 && next[i][1]==0 || next[i][0]==7 && next[i][1]==7){
            EvalVal_Corner = EvalVal_Corner + 2000*(limit+1);
        }

        //反転させるコマのリストを初期化
        for(int i=0;i<=59;i++){
            reverceState[i][0]=-1;
            reverceState[i][1]=-1;
        }
        reverceStateNum=0;  //反転させるコマの数を初期化

        //局面に子接点の手を打つ
        b23m6g26r_EvalReverce(next[i][0], next[i][1], turn );  //反転させるコマもカウントする

        //最近取ったコマを取られないようにする評価関数
        EvalVal_RevdNewMy = b23m6g26r_isReversed_newMyState(x, y, turn, i, lookup_RevdNewMy);
        
        //次の相手inの手.
        v = b23m6g26r_ab_min(x, y, turn, limit-1, alpha, beta, pass_Consecutive_Max2, EvalVal_Corner, EvalVal_Pass, EvalVal_RevdNewMy, lookup_RevdNewMy);

        //局面をもとに戻す. ローカル保存をもとにグローバルstateをもとに戻す
        b23m6g26r_state_SaveOrLoad_arg1Toarg2(state_save, state);  //state = state_save : load
        b23m6g26r_next_SaveOrLoad_arg1Toarg2(next_save, next);  //next = next_save : load
        
        if(v > alpha){
            alpha = v;
            if(alpha >= beta){
                return beta;  //カット
            }
        }   
    }
    return alpha;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//beta, 相手の手
int b23m6g26r_ab_min(int x, int y, int turn, int limit, int alpha, int beta, int pass_Consecutive_Max2, int EvalVal_Corner, int EvalVal_Pass, int EvalVal_RevdNewMy, int lookup_RevdNewMy[8][8])
{
    //printf("in ab_min()\n");
    cnt_explore[index_cnt_explore]++;  //探索回数のカウント

    int v;
    if(turn==1)	turn=2;
            else turn=1;

    //nextの初期化
    for(int i=0;i<=59;i++)
        for(int j=0;j<=1;j++)
            next[i][j]=-1;

    //nextの生成
    makeNextList( enemyTurn );

    //局面のローカル保存
    int state_save[8][8];
    int next_save[60][2];
    b23m6g26r_state_SaveOrLoad_arg1Toarg2(state, state_save);  //int state_save = state : save
    b23m6g26r_next_SaveOrLoad_arg1Toarg2(next, next_save);  //int next_save = next : save

    //自身と相手のコマ数のカウント.
    int myCount=0, enemyCount=0;
    if(myTurn==1){
        checkState(&myCount, &enemyCount);
    }else{
        checkState(&enemyCount, &myCount);
    }
        
    //自身、相手のコマがない
    int EvalVal_noCount = 0;
    if(enemyCount <= 0){
        EvalVal_noCount = EvalVal_noCount + 10000;
        //printf("turn:%d enemyCount<=0\n", turn);  //優先する, ローカル評価関数
        //exit(1);
        return EvalVal_noCount;

    }else if(myCount <= 0){
        EvalVal_noCount = EvalVal_noCount - 10000;
        //printf("turn:%d myCount<=0\n", turn);  //避ける
        //exit(1);
        return EvalVal_noCount;
    }

    //パスの確認
    int flag_noEnemyNext = 0;  //相手の次の手がない
    if(next[0][0]==-1){
        //printf("\nab_min : turn:%d pass\n", turn);  //パス
        //for(int passNumCount=0; passNumCount<20; passNumCount++) printf("pass");    
        //printf("\n");
        flag_noEnemyNext = 1;
        pass_Consecutive_Max2++;
    }

    //お互いにパス. 終局
    if(pass_Consecutive_Max2 > 1){
        //printf("pass_Consecutive_Max2 > 1\n");
        if(myCount > enemyCount){
            //printf("myCount > enemyCount\n");
            //printf("\nab_min : turn:%d pass\n", turn);  //パス
            //for(int passNumCount=0; passNumCount<20; passNumCount++) printf("pass");    
            //printf("\n");
            return 10000;
        }else{
            //printf("myCount <= enemyCount\n");
            //printf("\nab_min : turn:%d pass\n", turn);  //パス
            //for(int passNumCount=0; passNumCount<20; passNumCount++) printf("pass");    
            //printf("\n");
            return -10000;
        }
    }
        
    //評価関数
    if((limit == 0 || myCount+enemyCount >= 63 || myCount<=1)  && flag_noEnemyNext==0){
        if(mode == 0){
            return b23m6g26r_RandomAgent_FirstTime(&x, &y, turn, 5);  //先読みの確認
        
        }else{
            int ReturnEvaluation = 0;    //printf("ab_max() before EvalandPoly\n");    //printf("    &x, &y = %d, %d\n", &x, &y);    //printf("    x, y = %d, %d\n", x, y);    //printf("    *x, *y = %d, %d\n", *x, *y);

            fromAbmin = 1;
            //next_save, state_save, 評価値を渡して関数内で利用
            ReturnEvaluation = b23m6g26r_EvaluationAndPolynomial(&x, &y, turn, next_save, state_save, EvalVal_Corner, EvalVal_Pass, EvalVal_RevdNewMy, lookup_RevdNewMy);
            fromAbmin = 0;  //リセット
            
            return ReturnEvaluation;
        }
    }

    //エラー時の強制終了
    //終局ならエラーを出してもらう
    if(checkFinish()==1){
        //printf("error? : chechFinish in ab_min()\n");
        //printf("limit=%d\n", limit);
        //printf("enemyCount=%d, myCount=%d\n", enemyCount, myCount);
        //showState();
        //exit(1);
        return 0;
    }
    //limitの超過
    if(limit < 0){
        //printf("limit : %d\n", limit);
        //exit(1);
        return 0;
    }
    //nextに(1,9)はないのに置くので一旦next内の確認
    for(int i=0; next[i][0]!=-1; i++){
        if(next[i][0]<0 || next[i][0]>7 || next[i][1]<0 || next[i][1]>7){
            //printf("ab_min() next error : next[%d][0]=%d, next[%d][1]=%d\n", i, next[i][0], i, next[i][1]);
            //exit(1);
            return 0;
        }
    }

    //パスをするときの先読み
    if(flag_noEnemyNext==1){
        limit++;
        
        //パスの評価値の計算. 相手がパスをするのでプラス
        EvalVal_Pass = EvalVal_Pass + 1000*(limit+1);

        v = b23m6g26r_ab_max(x, y, turn, limit-1, alpha, beta, pass_Consecutive_Max2, EvalVal_Corner, EvalVal_Pass, EvalVal_RevdNewMy, lookup_RevdNewMy);
        
        //alpha-beta法
        if(v < beta){
            beta = v;
            if(beta <= alpha){
                return alpha;
            }
        }
        return beta;
    }
    //パスをせず、次の手を打つ時の先読み
    for(int i=0; next[i][0]!=-1; i++){
        pass_Consecutive_Max2 = 0;
        
        //打つ手(next)が角か否か
        if(next[i][0]==0 && next[i][1]==0 || next[i][0]==0 && next[i][1]==7 || next[i][0]==7 && next[i][1]==0 || next[i][0]==7 && next[i][1]==7){
            EvalVal_Corner = EvalVal_Corner - 2000*(limit+1);
        }

        // 反転させるコマのリストを初期化
        for(int i=0;i<=59;i++){
            reverceState[i][0]=-1;
            reverceState[i][1]=-1;
        }
        reverceStateNum=0;  //反転させるコマの数を初期化

        //局面に子接点の手を打つ
        b23m6g26r_EvalReverce(next[i][0], next[i][1], turn );  //反転させるマスもカウントする

        //最近取ったコマを取られないようにする評価関数
        EvalVal_RevdNewMy = b23m6g26r_isReversed_newMyState(x, y, turn, i, lookup_RevdNewMy);
        
        v = b23m6g26r_ab_max(x, y, turn, limit-1, alpha, beta, pass_Consecutive_Max2, EvalVal_Corner, EvalVal_Pass, EvalVal_RevdNewMy, lookup_RevdNewMy);

        //局面をもとに戻す
        b23m6g26r_state_SaveOrLoad_arg1Toarg2(state_save, state);  //state = state_save : load
        b23m6g26r_next_SaveOrLoad_arg1Toarg2(next_save, next);  //next = next_save : load


        if(v < beta){
            beta = v;
            if(beta <= alpha){
                return alpha;
            }
        }
    }
    return beta;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
//評価関数と多項式
//ランダム
int b23m6g26r_RandomAgent_FirstTime(int *x, int *y, int turn, int num){
//int RandomAgent(int *x, int *y, int turn){
    /* 現在の時刻を種に 0 〜 num-1 までの乱数を発生させて次の手を決定する（秒単位なので1秒以下の連続実行だと同じ値になる） */
    //x, yのconf
    //printf("RandomAgent_First\n");
    //sleep(1);
    srand((unsigned int)time(NULL));
    int random_i, random_num=num;
    random_i = rand() % random_num;
    
    /* 決定した手の座標を x,y にこの関数の結果として保存 */
    //i番目にnextに格納されたマスのx座標, y座標をx,yを代入している
    *x = next[random_i][0];
    *y = next[random_i][1];

}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//ランダム
int b23m6g26r_RandomAgent(int x, int y, int turn, int num){
    /* 現在の時刻を種に 0 〜 num-1 までの乱数を発生させて次の手を決定する（秒単位なので1秒以下の連続実行だと同じ値になる） */    //sleep(1);
    srand((unsigned int)time(NULL));
    int random_i, random_num=num;
    random_i = rand() % random_num;
    /* 決定した手の座標を x,y にこの関数の結果として保存 */
    //i番目にnextに格納されたマスのx座標, y座標をx,yを代入している
    x = next[random_i][0];    y = next[random_i][1];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
//仮でコマを置く関数. 反転させたコマの位置を保存する
void b23m6g26r_EvalReverce( int x, int y, int turn )
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

                    poss = 1;  //ここが0なのでエラーになる
                }
                count++;
            }
        }
        
        /* 相手の駒を反転させることができる場合 */
        if( poss == 1 ){
        
            /* 自分の駒に当たるまで反転を繰り返す */
            dist = 1;  //まずは1つ隣を調べる
            //*distにすることでdef=1ならさらにその先を調べ、def=0なら先を調べることはない    //state[][]==0とは、その先に駒がないということ
            while(	state[x+def[i][0]*dist][y+def[i][1]*dist] != turn &&
                    state[x+def[i][0]*dist][y+def[i][1]*dist] != 0) {
                
                check=1;
                state[x+def[i][0]*dist][y+def[i][1]*dist] = turn;  //反転
                //反転させるコマを格納
                reverceState[reverceStateNum][0] = x+def[i][0]*dist;
                reverceState[reverceStateNum][1] = y+def[i][1]*dist;
                reverceStateNum++;
                dist++;
            }
        }
    }    
    /* コマが置けないエラーが発生した場合は異常終了させる */
    if(check==0){
        showState();
        printf("x,y,turn = %d,%d,%d\n", x, y,turn);
        printf("tmpReverce() : ERROR\n");
    	printf("============= ERROR =============\n");
        exit(1);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
//state=state_save, state_save=stateを行う関数 
int b23m6g26r_state_SaveOrLoad_arg1Toarg2(int state1[8][8], int state2[8][8]){
    int i,j;
    for(i=0;i<8;i++){
        for(j=0;j<8;j++){
            state2[i][j]=state1[i][j];
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//next=next_save, next_save=nextを行う関数 
int b23m6g26r_next_SaveOrLoad_arg1Toarg2(int next1[60][2], int next2[60][2]){
    int i,j;
    for(i=0;i<60;i++){
        for(j=0;j<2;j++){
            next2[i][j]=next1[i][j];
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
//自分で手を打つ関数
int b23m6g26r_control(int *x, int *y, int turn){
    int i, j, num;
    int controlState;
    int candidateYX[60] = {-1};//候補との照合用リスト

    //デバッグ】プレーヤ情報を表示
    printf("Player(%d)\n", turn);
    
    //次候補リストを -1 で初期化する
    for(i=0;i<=59;i++)
        for(j=0;j<=1;j++)
            next[i][j]=-1;
    
    //プレーヤ番号を引数にして次候補リストを生成する
    makeNextList( turn );
    
    //候補数をカウントする
    num=0;
    while( next[num][0]!=-1 ) num++;

    //次手の候補がある場合
    if(num >= 1){
        //候補を2桁で表示. xy=11とか (左上の角を11)
        for(i=0;i<num;i++){
            candidateYX[i] = (next[i][1]+1)*10 + (next[i][0]+1);
            printf("%d ", candidateYX[i]);  //候補を2桁で表示. xy=11とか (左上の角を11
            //printf("%d%d ", next[i][1]+1, next[i][0]+1);
        }

        //2桁の数字で打ち手を入力
        while(1){
            printf("\n> ");
            scanf("%d", &controlState);

            if(controlState > 10 && controlState < 89){
                for(j=0; j<num; j++){
                    if(candidateYX[j] == controlState){
                        *y = (controlState/10)-1;
                        *x = (controlState%10)-1;
                        //printf("x=%d, y=%d\n", x, y);
                        goto loopend;  //下のloopendまで行く
                    }
                }
            }
            //printf("%d, %d\n", candidateYX[0], controlState);
            printf("候補の中から10以上の数字を入力してください. 左上の角は11です.\n");
        }
        loopend:
    
    } else {
    
        //置ける場所が無くパスするので -1 を結果として返す
        *x = -1;    *y = -1;
        
    }

}
////////////////////////////////////////////////////////////////////////////////////////////////////////
int b23m6g26r_Evaluation_MaxAndMin(int turn){
    
    //cornerのmax, min
    if(EVMaxMin[0][0] < EVNew[0]){  //max
        EVMaxMin[0][0] = EVNew[0];
    }else if(EVMaxMin[0][1] > EVNew[0]){  //min
        EVMaxMin[0][1] = EVNew[0];
    }
    //passのmax, min
    if(EVMaxMin[1][0] < EVNew[1]){  //max
        EVMaxMin[1][0] = EVNew[1];
    }else if(EVMaxMin[1][1] > EVNew[1]){  //min
        EVMaxMin[1][1] = EVNew[1];
    }
    //getStateMin
    if(EVMaxMin[2][0] < EVNew[2]){  //max
        EVMaxMin[2][0] = EVNew[2];
    }else if(EVMaxMin[2][1] > EVNew[2]){  //min
        EVMaxMin[2][1] = EVNew[2];
    }
    //getStateMax
    if(EVMaxMin[3][0] < EVNew[3]){  //max
        EVMaxMin[3][0] = EVNew[3];
    }else if(EVMaxMin[3][1] > EVNew[3]){  //min
        EVMaxMin[3][1] = EVNew[3];
    }
    //lookup : edge
    if(EVMaxMin[4][0] < EVNew[4]){  //max
        EVMaxMin[4][0] = EVNew[4];
    }else if(EVMaxMin[4][1] > EVNew[4]){  //min
        EVMaxMin[4][1] = EVNew[4];
    }
    //lookup : center
    if(EVMaxMin[5][0] < EVNew[5]){  //max
        EVMaxMin[5][0] = EVNew[5];
    }else if(EVMaxMin[5][1] > EVNew[5]){  //min
        EVMaxMin[5][1] = EVNew[5];
    }
    //lookup : sakai
    if(EVMaxMin[6][0] < EVNew[6]){  //max
        EVMaxMin[6][0] = EVNew[6];
    }else if(EVMaxMin[6][1] > EVNew[6]){  //min
        EVMaxMin[6][1] = EVNew[6];
    }
    //lookup : inverseSakai
    if(EVMaxMin[7][0] < EVNew[7]){  //max
        EVMaxMin[7][0] = EVNew[7];
    }else if(EVMaxMin[7][1] > EVNew[7]){  //min
        EVMaxMin[7][1] = EVNew[7];
    }
    //lookup : sakaiCorner
    if(EVMaxMin[8][0] < EVNew[8]){  //max
        EVMaxMin[8][0] = EVNew[8];
    }else if(EVMaxMin[8][1] > EVNew[8]){  //min
        EVMaxMin[8][1] = EVNew[8];
    }
    //lookup : bros
    if(EVMaxMin[9][0] < EVNew[9]){  //max
        EVMaxMin[9][0] = EVNew[9];
    }else if(EVMaxMin[9][1] > EVNew[9]){  //min
        EVMaxMin[9][1] = EVNew[9];
    }
    //lookup_board : edge
    if(EVMaxMin[10][0] < EVNew[10]){  //max
        EVMaxMin[10][0] = EVNew[10];
    }else if(EVMaxMin[10][1] > EVNew[10]){  //min
        EVMaxMin[10][1] = EVNew[10];
    }
    //lookup_board : center
    if(EVMaxMin[11][0] < EVNew[11]){  //max
        EVMaxMin[11][0] = EVNew[11];
    }else if(EVMaxMin[11][1] > EVNew[11]){  //min
        EVMaxMin[11][1] = EVNew[11];
    }
    //lookup_board : sakai
    if(EVMaxMin[12][0] < EVNew[12]){  //max
        EVMaxMin[12][0] = EVNew[12];
    }else if(EVMaxMin[12][1] > EVNew[12]){  //min
        EVMaxMin[12][1] = EVNew[12];
    }
    //lookup_board : inverseSakai
    if(EVMaxMin[13][0] < EVNew[13]){  //max
        EVMaxMin[13][0] = EVNew[13];
    }else if(EVMaxMin[13][1] > EVNew[13]){  //min
        EVMaxMin[13][1] = EVNew[13];
    }
    //lookup_board : sakaiCorner
    if(EVMaxMin[14][0] < EVNew[14]){  //max
        EVMaxMin[14][0] = EVNew[14];
    }else if(EVMaxMin[14][1] > EVNew[14]){  //min
        EVMaxMin[14][1] = EVNew[14];
    }
    //lookup_board : bros
    if(EVMaxMin[15][0] < EVNew[15]){  //max
        EVMaxMin[15][0] = EVNew[15];
    }else if(EVMaxMin[15][1] > EVNew[15]){  //min
        EVMaxMin[15][1] = EVNew[15];
    }
    //rateMore50_center
    if(EVMaxMin[16][0] < EVNew[16]){  //max
        EVMaxMin[16][0] = EVNew[16];
    }else if(EVMaxMin[16][1] > EVNew[16]){  //min
        EVMaxMin[16][1] = EVNew[16];
    }
    //weight_ifTakeCorner
    if(EVMaxMin[17][0] < EVNew[17]){  //max
        EVMaxMin[17][0] = EVNew[17];
    }else if(EVMaxMin[17][1] > EVNew[17]){  //min
        EVMaxMin[17][1] = EVNew[17];
    }
    //rateMore_MyPerEnemy_quadrant
    if(EVMaxMin[18][0] < EVNew[18]){  //max
        EVMaxMin[18][0] = EVNew[18];
    }else if(EVMaxMin[18][1] > EVNew[18]){  //min
        EVMaxMin[18][1] = EVNew[18];
    }
    //leaveOne
    if(EVMaxMin[19][0] < EVNew[19]){  //max
        EVMaxMin[19][0] = EVNew[19];
    }else if(EVMaxMin[19][1] > EVNew[19]){  //min
        EVMaxMin[19][1] = EVNew[19];
    }
    //MoreAndLess_spaceAround : more
    if(EVMaxMin[20][0] < EVNew[20]){  //max
        EVMaxMin[20][0] = EVNew[20];
    }else if(EVMaxMin[20][1] > EVNew[20]){  //min
        EVMaxMin[20][1] = EVNew[20];
    }
    //MoreAndLess_spaceAround : less
    if(EVMaxMin[21][0] < EVNew[21]){  //max
        EVMaxMin[21][0] = EVNew[21];
    }else if(EVMaxMin[21][1] > EVNew[21]){  //min
        EVMaxMin[21][1] = EVNew[21];
    }
    //MoreAndLess_nextNum : more
    if(EVMaxMin[22][0] < EVNew[22]){  //max
        EVMaxMin[22][0] = EVNew[22];
    }else if(EVMaxMin[22][1] > EVNew[22]){  //min
        EVMaxMin[22][1] = EVNew[22];
    }
    //MoreAndLess_nextNum : less
    if(EVMaxMin[23][0] < EVNew[23]){  //max
        EVMaxMin[23][0] = EVNew[23];
    }else if(EVMaxMin[23][1] > EVNew[23]){  //min
        EVMaxMin[23][1] = EVNew[23];
    }
    //rate50_board
    if(EVMaxMin[24][0] < EVNew[24]){  //max
        EVMaxMin[24][0] = EVNew[24];
    }else if(EVMaxMin[24][1] > EVNew[24]){  //min
        EVMaxMin[24][1] = EVNew[24];
    }
    //overlap_VertivalHorizontalDiagonal : more
    if(EVMaxMin[25][0] < EVNew[25]){  //max
        EVMaxMin[25][0] = EVNew[25];
    }else if(EVMaxMin[25][1] > EVNew[25]){  //min
        EVMaxMin[25][1] = EVNew[25];
    }
    //overlap_VertivalHorizontalDiagonal : less
    if(EVMaxMin[26][0] < EVNew[26]){  //max
        EVMaxMin[26][0] = EVNew[26];
    }else if(EVMaxMin[26][1] > EVNew[26]){  //min
        EVMaxMin[26][1] = EVNew[26];
    }
    //take_ExtremeOrHalfWay : more
    if(EVMaxMin[27][0] < EVNew[27]){  //max
        EVMaxMin[27][0] = EVNew[27];
    }else if(EVMaxMin[27][1] > EVNew[27]){  //min
        EVMaxMin[27][1] = EVNew[27];
    }
    //take_ExtremeOrHalfWay : less
    if(EVMaxMin[28][0] < EVNew[28]){  //max
        EVMaxMin[28][0] = EVNew[28];
    }else if(EVMaxMin[28][1] > EVNew[28]){  //min
        EVMaxMin[28][1] = EVNew[28];
    }
    //isReversed_newMyState
    if(EVMaxMin[29][0] < EVNew[29]){  //max
        EVMaxMin[29][0] = EVNew[29];
    }else if(EVMaxMin[29][1] > EVNew[29]){  //min
        EVMaxMin[29][1] = EVNew[29];
    }

}
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
