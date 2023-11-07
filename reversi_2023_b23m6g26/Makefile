#2023/06/16
#Makefile と呼ばれるテキストファイルに必要なファイルと
#各ファイルのコンパイルのコマンド, ファイル間の依存関係を記す. 
#そして, “make” というコマンドを実行するだけで, 自動的にコマンドを実行してコンパイルしてくれる.

OBJS = main.o display.o playGame.o playerCommon.o samplePlayer.o b23m6g26r.o
CC =	gcc

reversi : $(OBJS)
	$(CC) $(OBJS) -o reversi

main.o : main.c
	$(CC) -c main.c

display.o : display.c
	$(CC) -c display.c

playGame.o : playGame.c
	$(CC) -c playGame.c

playerCommon.o : playerCommon.c
	$(CC) -c playerCommon.c

samplePlayer.o : samplePlayer.c
	$(CC) -c samplePlayer.c

#myplayer
b23m6g26r.o : b23m6g26r.c
	$(CC) -c b23m6g26r.c

clean:
	rm -f $(OBJS)
