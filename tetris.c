#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "tetris.h"

void printStage(){
	int i;
	int j = 0;
	for(i = LWALL; i <= RWALL; i++){
		//mvaddch(LINES / 2 - HEIGHT / 2 - 1, i, ' ');
		//mvaddch(LINES / 2 + HEIGHT / 2 + 1, i, ' ');
		mvaddch(LINES / 2 - HEIGHT / 2 - 1, i, (j % 2 == 0)?'-':'=');
		mvaddch(LINES / 2 + HEIGHT / 2, i, (j++ % 2 == 0)?'-':'=');
	}
	for(i = LINES / 2 - HEIGHT / 2; i < LINES / 2 + HEIGHT / 2; i++){
		//mvaddch(i, LWALL, ' ');
  	//mvaddch(i, RWALL, ' ');
    mvaddch(i, LWALL, '|');
    mvaddch(i, RWALL, '|');
	}
}

void run(){
	int key;	//キー入力受け取り用の変数
	int score = 0;	//スコア

	Block moving, next;	//動いているブロックと次のブロック
	int stacked[HEIGHT * WIDTH];	//積まれたブロックを管理する

GAME_START:
	mvprintw(LINES/2+5,(COLS-10)/2,"start");
	mvprintw(LINES/2+7,(COLS-10)/2,"exit");
	for(;;){
		key = getch();
		if(key == 's'){
			clear();
			goto GAME;
		}else if(key == 'e'){
			return;
		}
	}
GAME:
	do{
		printStage();
	}while((key = getch()) != ESC);
	return;
}


int main(void){
	initscr();
	noecho();
	cbreak();
	keypad(stdscr,TRUE);
	run();
	endwin();
	return 0;
}
