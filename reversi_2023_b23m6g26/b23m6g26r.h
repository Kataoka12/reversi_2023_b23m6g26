/*
 *  samplePlayer.h
 *  Reversi
 *
 *  Created by Ryo OKAMOTO
 *  Copyright (c) All rights reserved.
 *
 */

void b23m6g26r( int *, int *, int );
//手動で打つための関数
int b23m6g26r_control(int *x, int *y, int turn);

//先読み用関数
int b23m6g26r_ab_max_FirstTime(int *x, int *y, int turn, int limit, int alpha, int beta);
int b23m6g26r_ab_max(int x, int y, int turn, int limit, int alpha, int beta, int pass_Consecutive_Max2, int EvalVal_Corner, int EvalVal_Pass, int EvalVal_RevdNewMy, int lookup_RevdNewMy[8][8]);
int b23m6g26r_ab_min(int x, int y, int turn, int limit, int alpha, int beta, int pass_Consecutive_Max2, int EvalVal_Corner, int EvalVal_Pass, int EvalVal_RevdNewMy, int lookup_RevdNewMy[8][8]);
//先読み用にstateとnextを保存する関数
int b23m6g26r_state_SaveOrLoad_arg1Toarg2(int state1[8][8], int state2[8][8]);
int b23m6g26r_next_SaveOrLoad_arg1Toarg2(int next1[60][2], int next2[60][2]);
//評価関数
int b23m6g26r_EvaluationAndPolynomial(int *x, int *y, int turn, int next_save[60][2], int state_save[8][8], int EvalVal_Corner, int  EvalVal_Pass, int EvalVal_RevdNewMy, int lookup_RevdNewMy[8][8]);
int b23m6g26r_getStateMin(int x, int y, int turn, int myCount_old, int myCount_new);
int b23m6g26r_getStateMax(int x, int y, int turn, int myCount_old, int myCount_new);
int b23m6g26r_lookup(int x, int y, int turn, int nextI);  //すべてのlookupの評価値を計算する関数
int b23m6g26r_lookup_board(int x, int y, int turn); //盤面に対するlookup
int b23m6g26r_rateMore50_center(int x, int y, int turn);  //中心(4×4)の中にある自身のコマの割合を5割以上に維持する
int b23m6g26r_weight_ifTakeCorner(int x, int y, int turn, int nextI);  //取っている角に近いマスほど大きな重みをつける
int b23m6g26r_rateMore_MyPerEnemy_quadrant(int x, int y, int turn, int nextI, int state_save[8][8]);  //コマの数において自身/相手の割合が高い象限の評価関数
int b23m6g26r_leaveOne(int x, int y, int turn, int nextI);  //相手のマスが周囲に多い場所を避ける. 端は重み強め.
int b23m6g26r_MoreAndLess_spaceAround(int x, int y, int turn, int nextI, int mode);  //周りのマスに空きが多いほど高評価、空きが少ないほど高評価. mode=0:空きが多いほど, mode=1：空きが少ないほど, mode=2:両方
int b23m6g26r_MoreAndLess_nextNum(int x, int y, int turn, int nextI, int mode);  //手数を多く、少なく. mode=0:多く, mode=1:少なく, mode=2:両方
int b23m6g26r_rate50_board(int x, int y, int turn);  //盤面全体で自身のコマ:相手のコマ=1:1を維持する
int b23m6g26r_overlap_VertivalHorizontalDiagonal(int x, int y, int turn, int nextI, int state_save[8][8], int mode);  //縦横斜めの被り多い、少ない. mode=0:多く, mode=1:少なく, mode=2:両方
int b23m6g26r_take_ExtremeOrHalfWay(int x, int y, int turn, int myCount_old, int myCount_new, int mode);  //極端な個数とる手、中途半端な個数とる手を優先する
int b23m6g26r_isReversed_newMyState(int x, int y, int turn, int nextI, int lookup_RevdNewMy[8][8]);  //最近置いた、反転させたコマを反転されづらく
//ランダムに手を打つ関数
int b23m6g26r_RandomAgent(int x, int y, int turn, int num);
int b23m6g26r_RandomAgent_FirstTime(int *x, int *y, int turn, int num);
//先読みでコマを仮置きする関数
void b23m6g26r_EvalReverce( int x, int y, int turn );  //評価関数の中でコマをひっくり返す際のReverce関数
int b23m6g26r_Evaluation_MaxAndMin(int turn);  //評価値の最大値と最小値を更新する関数. ここで更新した値を元に評価値を調整する
