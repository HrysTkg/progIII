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
	int blockInitX = LWALL + WIDTH / 2;
	int blockInitY = LINES / 2 - HEIGHT / 2;
	next.pos.x = COLS / 2 + 10;
	next.pos.y = LINES / 2;

	int stacked[WIDTH][HEIGHT] = {};	//積まれたブロックを管理する

	Point types[7][4] = {TYPE1,TYPE2,TYPE3,TYPE4,TYPE5,TYPE6,TYPE7}; 
	int i,k;

	timeout(200);
	srand((unsigned int)time(NULL));
	k = rand() % 7;

GAME_START:
	mvprintw(LINES/2+5,(COLS-10)/2,"[s] : start");
	mvprintw(LINES/2+7,(COLS-10)/2,"[ESC] : exit");
	for(;;){
		key = getch();
		if(key == 's'){
			clear();
			goto GAME;
		}else if(key == ESC){
			return;
		}
	}
GAME:
	nextBlock(&moving, &next, types);
	nextBlock(&moving, &next, types);
	while((key = getch()) != ESC){
		clearBlock(moving);
		clearBlock(next);
		printStage();

		if(isHitBottom(moving)){
			nextBlock(&moving, &next, types);
		} else {
			moveBlock(&moving, key);
		}
		printBlock(moving);
		printBlock(next);
	}
	return;
}

void printBlock(Block b){
	int i;
	for(i = 0; i < 4; i++){
		mvaddch(b.pos.y + b.bp[i].y, b.pos.x + b.bp[i].x, '@');
	}
}

void clearBlock(Block b){
	int i;
	for(i = 0; i < 4; i++){
		mvaddch(b.pos.y + b.bp[i].y, b.pos.x + b.bp[i].x, ' ');
	}
}

void moveBlock(Block* b, int key){
	switch(key){
	case KEY_LEFT:
		if(isHitLWall(*b) == 0)
			b->pos.x -= 1;
		break;
	case KEY_RIGHT:
		if(isHitRWall(*b) == 0)
			b->pos.x += 1;
		break;
	case KEY_UP:
		rotateBlock(b);
		break;
	default:
		if(isHitBottom(*b) == 0)
			b->pos.y += 1;
	}
}

void rotateBlock(Block* b){
	int i;
	for(i = 0; i < 4; i++){
		int tempX = b->bp[i].x;
		int tempY = b->bp[i].y;
		b->bp[i].x = tempY;
		b->bp[i].y = -1 * tempX;
	}
}

void nextBlock(Block* mov, Block* nxt, Point types[7][4]){
	int i,k;
	k = rand() % 7;

	mov->pos.x = LWALL + WIDTH / 2;
        mov->pos.y = LINES / 2 - HEIGHT / 2;
	for(i = 0; i < 4; i++){
		mov->bp[i] = nxt->bp[i];
		nxt->bp[i] = types[k][i];
	}
}

int isHitBottom(Block b){
	int btm = LINES / 2 + HEIGHT / 2;
        int i;
        for(i = 0; i < 4; i++){
                if((b.pos.y + b.bp[i].y + 1) == btm)
                        return 1;
        }
        return 0;
}

int isHitLWall(Block b){
        int i;
        for(i = 0; i < 4; i++){
                if((b.pos.x + b.bp[i].x - 1) == LWALL)
                        return 1;
        }
        return 0;
}

int isHitRWall(Block b){
        int i;
        for(i = 0; i < 4; i++){
                if((b.pos.x + b.bp[i].x + 1) == RWALL)
                        return 1;
        }
        return 0;
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
