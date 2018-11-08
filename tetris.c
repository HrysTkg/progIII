#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "tetris.h"

void printStage(){
	int i;
	for(i = 0; i < LINES; i++){
		mvaddch(i,LWALL,' ');
                mvaddch(i,RWALL,' ');
                mvaddch(i,LWALL,'|');
                mvaddch(i,RWALL,'|');
	}
}

void run(){
	int key;	//キー入力受け取り用の変数
 
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
	while((key = getch()) != 'e'){
		printStage();
	}
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
