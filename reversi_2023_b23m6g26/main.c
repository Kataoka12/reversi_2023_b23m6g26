/*
 *  main.c
 *  Reversi
 *
 *  Created by Ryo OKAMOTO
 *  Copyright (c) All rights reserved.
 *
 */

#include <stdio.h>			/* printf の warning を避けるため	*/
#include "playGame.h"
#include "display.h"

/*【重要】外部変数に現在の状態を保存 */
int state[8][8];

/*【重要】外部変数に次候補リストを保存 */
int next[60][2];    // 行ごとに候補のX座標（列1）とY座標（列2）を格納

//乱数
#include <stdlib.h>			
#include <time.h>
#include <unistd.h>  //sleep関数を使うために必要
#include <windows.h>

int myWinNum = 0; //勝利回数
int myLoseNum = 0; //敗北回数
int myDrawNum = 0; //引き分け回数
int cnt_explore_main[101];  //探索の回数をカウントする. mini-max, alpha-beta法の比較に使用
int index_cnt_explore_main = 0;  //cnt_exploreのインデックス

int EVMaxMin_main[35][2] = {100};  //EvalVal_MaxAndMin. [][0]がmax, [][1]がmin
int EVNew_main[35] = {100};  //EvalVal_New. 最新の評価値を格納する. EvalVal_MaxAndMinと比較して最大値と最小値を更新する

//各評価関数の係数
float coef_EvalFunc_main[10][30];  //遺伝的アルゴリズムで変化する. [プレイヤー番号][各評価値の係数]
float coef_EvalFunc_one[30] = {
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
};  //全ての係数1. 各評価関数の係数. 遺伝的アルゴリズムでも変化しない
float coef_EvalFunc_winRate[30] = {
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
};  //勝率による係数. 各評価関数の係数. 遺伝的アルゴリズムでも変化しない
float coef_EvalFunc_winRateSub05[30] = {
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
};  //勝率による係数. 勝率-0.5. 5割以上の勝率なら正、5割り未満なら負. 各評価関数の係数. 遺伝的アルゴリズムでも変化しない
float coef_EvalFunc_GA[30] = {
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
};  //遺伝的アルゴリズムで生成された係数. 各評価関数の係数. 遺伝的アルゴリズムでも変化しない
//main_base() : 元々のmain
int b23m6g26r_main_base (int argc, const char * argv[]);
//main_GA() : 遺伝的アルゴリズムを導入したmain
int b23m6g26r_main_GA (int argc, const char * argv[]);
//main_GAconfirmation() : 遺伝的アルゴリズムで生成した係数を適応して対戦して強さの検証を行うmain
int b23m6g26r_main_GAconfirmation (int argc, const char * argv[]);
//
//
//講義での対戦で使用する係数
float coef_EvalFunc_base[10][30] = {
    {32.762024, 21.723972, -17.138981, 19.378000, 36.681965, -18.018013, -40.259991, 45.684994, 42.850952, 89.012047, 67.133003, -21.459002, 30.828005, 38.318020, 4.774992, -3.218011, -37.714996, 14.383005, -33.882999, 24.117018, 7.818003, 10.031981, 23.242002, -27.717999, 18.810972, 17.310999, -40.779953, -45.294987, -10.060990, -45.074913},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};  //各評価関数の係数. 遺伝的アルゴリズムでも変化しない
int playerNum_main = 0;  //coef_EvalFunc_main[][]の１次元目のプレイヤー番号. turnのようにb23m6g26r.c内で変化することはない. playGame.cのturnと対応しているイメージ
//
//

/*--------------------------------
    メイン関数
 ---------------------------------*/
int main (int argc, const char * argv[]) {

    
    //playGame.c内をb23m6g26r()やsamplePlayer()に変更することで対戦相手を変更しておく

    int flag=0;
    
    if(flag==0){
        b23m6g26r_main_base (argc, argv);  //b23mr6g26r.c vs samplePlayer用
    }else if(flag==1){
        b23m6g26r_main_GA (argc, argv);  //遺伝的アルゴリズム用
    }else{
        b23m6g26r_main_GAconfirmation(argc, argv);  //b23mr6g26r.c vs b23mr6g26r.c用（係数違い）
    }
    
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
//もともとのmain（2023/07/25, 20:00以前）
int b23m6g26r_main_base (int argc, const char * argv[]) {

    /* 先手: turn=1，後手: turn=2 */
    //私を後手(player2)にしている. ランダムで先攻後攻が決まるようにはしたい.
    //int turn_init = 1;  //ループの中での初期化でも先攻を維持するため
    int turn = 1;
    int myTurn = 2;  //playGame.c内でelse側なので
    printf("turn = %d\n", turn);//conf
    //float計算の確認    //float a = 3 / 5.0;    //int b = (int)(3/5 * 100);    //printf("%f %f\n", a, b);    //exit(1);

    /* 連続したパスの回数 */
    int pass = 0;

    //start地点がわかるように
    printf("//////////////////////////////////////////////////////////////////////////////////////////////////\n");
    
    /* 局面の初期化と表示 */
    initState();

    //勝敗のカウント用コマ数カウント
    int myCount=0, enemyCount=0;

    //
    //元々のmainではplayerNum=0のままかな. 
    //forループでcoef_EvalFunc_main[][]とcoef_EvalFunc_base[]初期化とかもして.
    //
    //遺伝的アルゴリズムに合わせて評価関数の係数の決定
    //最初のプレイヤーと係数を生成. 遺伝的アルゴリズム
    //coef_EvalFunc_base[][30]から、coef_EvalFunc_main[0][30]のみ作成
    for(int i=0;i<30;i++){
        coef_EvalFunc_main[0][i] = coef_EvalFunc_base[0][i];  //ここを変更すれば評価関数の係数を変更できる
    }
    playerNum_main = 0;  //b23m6g26rは1つなので常にplayerNum=0
    
    //n回繰り返す
    int GameLoopNum = 100;
    for(int i=0; i<GameLoopNum; i++){
    
        /* 終局まで処理を繰り返す */
        do {        
            /* 手を進める（パスの場合は-1，正常で 0 が戻る） */
            if( proceedGame( turn ) == -1 ){  //in"PlyaGame.c", Player(x)を表示
                pass++;
                printf("------PASS------\n");
            } else pass = 0;
                
            /* 局面を表示 */  //局面とA、B、Totalの表示
            //最終回だけ表示
            //if(i==GameLoopNum-1){
              showState();  //in"display.c"
            //}

            /* プレーヤを交代 */
            if(turn==1)	turn=2;
                else turn=1;
        
        /* 終局であるかどうかを調べる（２回連続パスした場合も終了する） */
        //2回連続でパス＝お互いにおける場所がない.    //終局条件を満たせばcheckFinish()=1なので、!1=0=Falseとなり、do-while文を抜ける
        //両方が真ならループを続ける
        } while( ! checkFinish() && pass != 2);  //in"playGame.c : checkFinish()"
        
        //勝敗の確認
        myCount=0, enemyCount=0;

        //第１引数はturn==1の方、第２引数はturn==2の方
        if(myTurn == 1){
            checkState(&myCount, &enemyCount);
        }else{
            checkState(&enemyCount, &myCount);  //in"playGame.c : checkState()"
        }
        //第一引数はplayGeme.cのturn==1なので相手

        //showState();  //in"display.c"

        
        if(enemyCount < myCount){
            printf("I win!\n");
            myWinNum++;
        }else if(enemyCount > myCount){
            printf("I lose...\n");
            myLoseNum++;
        }else{
            printf("Draw. I Win?\n");
            myDrawNum++;
        }
        printf("win, lose, draw = %d, %d, %d\n", myWinNum, myLoseNum, myDrawNum);
        
        
        //先攻後攻のリセット。一旦先攻=Player1
        if(i<(GameLoopNum/2)){
            turn = 1;
        }else{
            turn = 2;
        }

        pass = 0;
        index_cnt_explore_main++;

        //samplePlayerの乱数の変更のために。やってもやらなくても強さに差がない。時間がかかるだけ。
        sleep(1);  //1秒待つ

        //start地点がわかるように
        //printf("//////////////////////////////////////////////////////////////////////////////////////////////////\n");
        /* 局面の初期化と表示 */
        initState();

    }
    
    //finish地点がわかるように
    printf("//////////////////////////////////////////////////////////////////////////////////////////////////\n");
    printf("myWinNum = %d\n", myWinNum);
    printf("myLoseNum = %d\n", myLoseNum);
    printf("myDrawNum = %d\n", myDrawNum);
    printf("0~49:turn=1, 50~99:turn=2\n");

    //探索の回数を表示
    /*
    int all_cnt_explore = 0;
    for(int i=0; i<index_cnt_explore_main; i++){
        all_cnt_explore = all_cnt_explore + cnt_explore_main[i];
    }
    printf("alpha-beta loop1\n");
    printf("explore %d times\n", all_cnt_explore/index_cnt_explore_main);

    printf(" num         max    min\n");
    for(int i=0; i<30; i++){
        printf(" %2d   %7d   %7d\n", i, EVMaxMin_main[i][0], EVMaxMin_main[i][1]);
    }
    */
    
    
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
int b23m6g26r_main_GA (int argc, const char * argv[]){

    int conf_num=0;
    conf_num++;  //確認
    printf("conf_num = %d\n", conf_num);

    /* 先手: turn=1，後手: turn=2 */
    //int turn_init = 1;  //ループの中での初期化でも先攻を維持するため
    int turn = 1;
    /* 連続したパスの回数 */
    int pass = 0;
    srand((unsigned int)time(NULL));

    //start地点がわかるように
    //printf("//////////////////////////////////////////////////////////////////////////////////////////////////\n");
    /* 局面の初期化と表示 */
    initState();


    //coef_EvalFunc_base[][]をcoef_EvalFunc_main[][]にロード、作成
    for(int i=0;i<10;i++){
        for(int j=0;j<30;j++){
            coef_EvalFunc_main[i][j] = coef_EvalFunc_base[i][j];
        }
    }
    
    float coef_changeMax = 1.0; //変異の最大変異量
    int coef_changeIndex;  //変化させる係数の決定. 0~29の乱数を生成    
    int random;  //残りの変異量未満の乱数を生成
    int random_denominator = 0;
    float coef_change;  //0~1の乱数を生成
    int AddOrSub;
    float coef_EvalFunc_new[10][30];  //coef_EvalFunc_main[main[][]を更新するために一時的に保存するリスト. 遺伝的アルゴリズムで変化する
    
    //対戦, 遺伝的アルゴリズム. 手動で止めるまで繰り返す
    int loopNum = 0;
    int loopNumMax = 1;
    FILE *fp;  //ファイル書き込み用

    //while(loopNum < loopNumMax){
    while(1){
        loopNum++;

        //各回の総当たり用ランダムの生成 -> randomを使う前に1回下でやっている
        //srand((unsigned int)time(NULL));  //whileを1周するのに10秒ぐらいはかかる

        //各プレイヤーの勝利数
        int winNum[10] = {0};
        int p1_count=0, p2_count=0;  //各プレイヤーのコマ数カウント用
        
        //
        //総当たり. 90局. winNum[10]の更新
        //
        for(int p1=0; p1<10; p1++){
            for(int p2=p1+1; p2<10; p2++){
                
                //対戦1. p1先攻, p2後攻
                turn=1;
                playerNum_main=p1;
                for(int i=0; i<2; i++){  //先攻後攻を入れ替えるためi<2
                    
                    /* 終局まで処理を繰り返す */
                    do {
                        /* 手を進める（パスの場合は-1，正常で 0 が戻る） */
                        if( proceedGame( turn ) == -1 ){  //in"PlyaGame.c", Player(x)を表示
                            pass++;
                            printf("------PASS------\n");
                        } else pass = 0;
                            
                        /* 局面を表示 */  //局面とA、B、Totalの表示    //最終回だけ表示    //if(i==GameLoopNum-1){showState();}  //in"display.c"

                        /* プレーヤを交代 */
                        if(turn==1)	turn=2;
                            else turn=1;
                        //coef_EvalFunc_main[][]のプレイヤー番号（１次元目）の更新
                        if(playerNum_main==p1) playerNum_main=p2;
                            else playerNum_main=p1;
                        
                    
                    /* 終局であるかどうかを調べる（２回連続パスした場合も終了する） */
                    //2回連続でパス＝お互いにおける場所がない    //終局条件を満たせばcheckFinish()=1なので、!1=0=Falseとなり、do-while文を抜ける    //両方が真ならループを続けるように、&&を使っている
                    } while( ! checkFinish() && pass != 2);  //in"playGame.c : checkFinish()"

                    //勝利数カウント.  winNum[10];
                    checkState(&p1_count, &p2_count);
                    if(p1_count > p2_count){
                        winNum[p1]++;
                    }else if(p1_count < p2_count){
                        winNum[p2]++;
                    }else{
                        //引き分け
                    }

                    //確認
                    //printf("p1_count, p2_count = %d, %d\n", p1_count, p2_count);
                    //printf("winNum[%d], winNum[%d] = %d, %d\n", p1, p2, winNum[p1], winNum[p2]);
                    
                    //初期化
                    p1_count=0, p2_count=0;
                    pass = 0;
                    index_cnt_explore_main++;
                    
                    initState();  //局面の初期化と表示
                    //sleep(1);  //1秒待つ
                    
                    //対戦2. p2先攻, p1後攻
                    turn=2;  //先攻後攻入れ替え
                    playerNum_main=p2;
                }

            }
        }
        index_cnt_explore_main = 0;  //初期化

        //ランダムシードの更新
        srand((unsigned int)time(NULL));
        
        //
        //勝利数をもとに、最強のプレイヤーを決定
        //
        int playerNum_maxWin = 0;  //勝利数が最も多いプレイヤーの番号
        int winNum_max = winNum[0];  //最も多い勝利数
        int playerNum_equalWin[10] = {0};  //勝利数が同じプレイヤーを格納
        int index_equalWin = 0;
        for(int i=0; i<10; i++){
            if(winNum_max < winNum[i]){
                winNum_max = winNum[i];  //最も多い勝利数を更新
                playerNum_maxWin = i;    //最も多い勝利数のプレイヤー番号を更新

                //初期化
                index_equalWin = 0;
                playerNum_equalWin[index_equalWin] = i;

            }else if(winNum_max == winNum[i]){
                index_equalWin++;
                playerNum_equalWin[index_equalWin] = i;
            }
        }
        //同じ勝利数のプレイヤーがいる場合は、ランダムに選択
        if(index_equalWin > 0){
            //int random;
            random = rand() % (index_equalWin+1);  //0~index_equalWinの乱数を生成
            playerNum_maxWin = playerNum_equalWin[random];    
        }
        

        //
        //遺伝的アルゴリズム
        //coef_EvalFunc_new[][]の更新, coef_EvalFunc_new[1][30] ~ [9][30]を作成
        //
        //coef_EvalFunc_new[0][30]に最も勝利数が多いプレイヤーの係数を保存する
        for(int i=0;i<30;i++){
            coef_EvalFunc_new[0][i] = coef_EvalFunc_main[playerNum_maxWin][i];
        }

        //交叉用ループ
        for(int k=1;k<10;k++){
            int winNum_sum = 0;  //勝利数の合計
            int winNum_roulette10[10];  //
            for(int i=0;i<10;i++){
                winNum_sum = winNum_sum + winNum[i];
                winNum_roulette10[i] = winNum_sum;  //勝利数が5,7,3ならrouletteは5,12,15で、ランダムで13が出力されたらプレイヤー2が選択されたということ. プレイヤー0から考えて.
            }
            //交叉を行うプレイヤーの決定. 同じプレイヤーが2回選ばれることもある.
            int playerNum_cross[2];
            //srand((unsigned int)time(NULL));  //ここで行うと同じtimeから同じシードが生成される
            for(int i=0;i<2;i++){
                //int random;
                random = rand() % winNum_sum;  //0~winNum_sum-1の乱数を生成
                for(int j=0;j<10;j++){
                    if(random < winNum_roulette10[j]){
                        playerNum_cross[i] = j;  //交叉するプレイヤーの決定. プレイヤー番号を格納
                        break;
                    }
                }
            }
            //交叉
            //int winNum_roulette2[2];
            winNum_sum = winNum[playerNum_cross[0]] + winNum[playerNum_cross[1]];
            for(int i=0;i<30;i++){
                //int random;
                random = rand() % winNum_sum;  //0~winNum_sum-1の乱数を生成
                
                //coef_EvalFunc_new[k][30]の作成. winNum_roulette2[2]
                if(random < winNum[playerNum_cross[0]]){
                    coef_EvalFunc_new[k][i] = coef_EvalFunc_main[playerNum_cross[0]][i];
                }else{
                    coef_EvalFunc_new[k][i] = coef_EvalFunc_main[playerNum_cross[1]][i];
                }
            }


        }
        //変異  //coef_EvalFunc_new[1][30] ~ [9][30]の調整
        //変異量の決定
        float sum_coef_EFnew = 0;
        for(int i=0;i<10;i++){
            for(int j=0;j<30;j++){
                if(coef_EvalFunc_new[i][j] > 0.0){
                    sum_coef_EFnew = sum_coef_EFnew + coef_EvalFunc_new[i][j];
                }else{
                    sum_coef_EFnew = sum_coef_EFnew - coef_EvalFunc_new[i][j];
                }
            }
        }
        for(int i=1;i<10;i++){  //[0][]は変異しない
            //初期化
            //srand((unsigned int)time(NULL));  //ここで行うと同じtimeから同じシードが生成される
            coef_changeMax = 1.0;
            //coef_changeMax = sum_coef_EFnew / 100.0;

            while(coef_changeMax >= 0.0){
                //変化させる係数の決定
                coef_changeIndex = rand() % 30;

                //変異量の決定
                random_denominator = (int)(coef_changeMax * 1000.0);
                if(random_denominator != 0){
                    random = rand() % random_denominator;
                }else{
                    random = 0;
                }

                coef_change = (random+1) / 1000.0;  //0~coef_changeMaxの乱数を生成

                //残りの変異量の更新
                coef_changeMax = coef_changeMax - coef_change;

                //加算、減算の決定
                AddOrSub = rand() % 2;  //0, 1の乱数を生成
                
                //変異
                if(AddOrSub == 0){  //add
                    coef_EvalFunc_new[i][coef_changeIndex] = coef_EvalFunc_new[i][coef_changeIndex] + coef_change;
                }else{  //sub
                    coef_EvalFunc_new[i][coef_changeIndex] = coef_EvalFunc_new[i][coef_changeIndex] - coef_change;
                }
            }
        }
        //突然変異.   //coef_EvalFunc_new[1][30] ~ [9][30]の調整
        //プレイヤー1からプレイヤー9に対して. 最大変位量10
        coef_changeMax = 10.0;
        //coef_changeMax = sum_coef_EFnew / 10.0;
        int playerNum_mutation = 0;  //突然変異を行うプレイヤーの決定
        for(int i=0;i<3;i++){  //3回突然変異を行う
            playerNum_mutation = (rand() % 9) + 1;  //突然変異を行うプレイヤーの決定  //1~9の乱数を生成
            while(coef_changeMax >= 0.0){
                //変化させる係数の決定
                coef_changeIndex = rand() % 30;

                //変異量の決定
                random_denominator = (int)(coef_changeMax * 1000.0);
                if(random_denominator != 0){
                    random = rand() % random_denominator;
                }else{
                    random = 0;
                }

                coef_change = (random+1) / 1000.0;  //0~coef_changeMaxの乱数を生成

                //残りの変異量の更新
                coef_changeMax = coef_changeMax - coef_change;

                //加算、減算の決定
                AddOrSub = rand() % 2;  //0, 1の乱数を生成
                
                //変異
                if(AddOrSub == 0){  //add
                    coef_EvalFunc_new[playerNum_mutation][coef_changeIndex] = coef_EvalFunc_new[playerNum_mutation][coef_changeIndex] + coef_change;
                }else{  //sub
                    coef_EvalFunc_new[playerNum_mutation][coef_changeIndex] = coef_EvalFunc_new[playerNum_mutation][coef_changeIndex] - coef_change;
                }
            }
        }

        //~_new[][]をcoef_EvalFunc_main[][]にコピー. ~new[0][]は最も勝利数の多かったプレイヤーがそのまま入っている
        for(int i=0;i<10;i++){
            for(int j=0;j<30;j++){
                coef_EvalFunc_main[i][j] = coef_EvalFunc_new[i][j];
            }
        }

        //
        //係数のファイル保存
        //
        fp = fopen("coef_GeneticAlgorithm.txt", "a");  //書き込みモードでファイルを開く
        fprintf(fp, "%d\n", loopNum);
        for(int i=0;i<10;i++){
            for(int j=0;j<29;j++){
                fprintf(fp, "%f, ", coef_EvalFunc_main[i][j]);  //開いたファイルに書き込む
            }
            fprintf(fp, "%f\n", coef_EvalFunc_main[i][29]);  //各coef_EvalFunc_main[][]で改行. ~_base[]へのコピーを簡単にするため最後はここで書き込み
        }
        fprintf(fp, "\n");  //各世代でスペースを空ける
        fclose(fp);  //ファイルを閉じる. 閉じていた方がループ中でもファイル操作をしやすそう

        //
        //次の対戦を行う
        //

    }

    return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//遺伝的アルゴリズムで生成した係数を適応して対戦し強さの検証を行うmain
int b23m6g26r_main_GAconfirmation (int argc, const char * argv[]) {

    /* 先手: turn=1，後手: turn=2 */
    int turn_init = 1;  //ループの中での初期化でも先攻を維持するため
    int turn = turn_init;
    int myTurn = 2;  //playGame.c内でelse側なので
    
    /* 連続したパスの回数 */
    int pass = 0;

    //start地点がわかるように
    //printf("//////////////////////////////////////////////////////////////////////////////////////////////////\n");
    
    /* 局面の初期化と表示 */
    initState();

    //勝敗のカウント用コマ数カウント
    int myCount=0, enemyCount=0;

    //検証する係数を2つ選択
    for(int i=0;i<30;i++){  //相手
        coef_EvalFunc_main[0][i] = coef_EvalFunc_one[i];  //配列名や係数を変更
    }
    for(int i=0;i<30;i++){  //自分
        coef_EvalFunc_main[1][i] = coef_EvalFunc_base[0][i];  //配列名や係数を変更
    }
    
    if(turn==1){
        playerNum_main = 0;  //turn=1 : playerNum_main=0
    }else{
        playerNum_main = 1;  //turn=2 : playerNum_main=1
    }
    
    //n回繰り返す
    int GameLoopNum = 10;
    for(int i=0; i<GameLoopNum; i++){
        // 現在の時刻を種に 0 〜 num-1 までの乱数を発生させて次の手を決定する（秒単位なので1秒以下の連続実行だと同じ値になる）
        //srand((unsigned int)time(NULL));
    
        /* 終局まで処理を繰り返す */
        do {        
            /* 手を進める（パスの場合は-1，正常で 0 が戻る） */
            if( proceedGame( turn ) == -1 ){  //in"PlyaGame.c", Player(x)を表示
                pass++;
                printf("------PASS------\n");
            } else pass = 0;
                
            /* 局面を表示 */  //局面とA、B、Totalの表示
            //最終回だけ表示
            //if(i==GameLoopNum-1){
              showState();  //in"display.c"
            //}

            /* プレーヤを交代 */
            if(turn==1)	turn=2;
                else turn=1;
            if(playerNum_main==0) playerNum_main=1;
                else playerNum_main=0;
        
        /* 終局であるかどうかを調べる（２回連続パスした場合も終了する） */
        //2解連続でパス＝お互いにおける場所がない. 終局条件を満たせばcheckFinish()=1なので、!1=0=Falseとなり、do-while文を抜ける
        //両方が真ならループを続ける
        } while( ! checkFinish() && pass != 2);  //in"playGame.c : checkFinish()"
        
        //勝敗の確認
        myCount=0, enemyCount=0;

        //第１引数はturn==1の方、第２引数はturn==2の方
        if(myTurn == 1){
            checkState(&myCount, &enemyCount);
        }else{
            checkState(&enemyCount, &myCount);  //in"playGame.c : checkState()"
        }
        //第一引数はplayGeme.cのturn==1なので相手

        //showState();  //in"display.c"
        /*
        if(enemyCount < myCount){
            printf("I win!\n");
            myWinNum++;
        }else if(enemyCount > myCount){
            printf("I lose...\n");
            myLoseNum++;
        }else{
            printf("Draw. I Win?\n");
            myDrawNum++;
        }
        printf("win, lose, draw = %d, %d, %d\n", myWinNum, myLoseNum, myDrawNum);
        */
        //先攻後攻のリセット。一旦先攻=Player1
        if(i<(GameLoopNum/2)){
            turn = 1;
        }else{
            turn = 2;
        }

        pass = 0;
        index_cnt_explore_main++;

        sleep(1);  //1秒待つ

        //start地点がわかるように
        //printf("//////////////////////////////////////////////////////////////////////////////////////////////////\n");
        /* 局面の初期化と表示 */
        initState();

    }
    //finish地点がわかるように
    /*
    printf("//////////////////////////////////////////////////////////////////////////////////////////////////\n");
    printf("myWinNum = %d\n", myWinNum);
    printf("myLoseNum = %d\n", myLoseNum);
    printf("myDrawNum = %d\n", myDrawNum);
    printf("0~49:turn=1, 50~99:turn=2\n");

    //探索の回数を表示
    int all_cnt_explore = 0;
    for(int i=0; i<index_cnt_explore_main; i++){
        all_cnt_explore = all_cnt_explore + cnt_explore_main[i];
    }
    printf("alpha-beta loop1\n");
    printf("explore %d times\n", all_cnt_explore/index_cnt_explore_main);

    printf(" num         max    min\n");
    for(int i=0; i<30; i++){
        printf(" %2d   %7d   %7d\n", i, EVMaxMin_main[i][0], EVMaxMin_main[i][1]);
    }
    */
    
    return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////