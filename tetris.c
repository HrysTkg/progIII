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

	timeout(WAIT_TIME);
	srand((unsigned int)time(NULL));
	k = rand() % 7;

GAME_START:
	mvprintw(LINES/2+5,(COLS-10)/2,"[s] : start");
	mvprintw(LINES/2+7,(COLS-10)/2,"[Esc] : exit");
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
	//メインループ
	while((key = getch()) != ESC){
		clear();
		//clearBlock(moving);
		//clearBlock(next);
		printStage();
		Block cp = moving;
		cp.pos.y += 1;
		if(isEnableBlock(cp, stacked) == 0){
			addStackedBlock(stacked, moving);
			nextBlock(&moving, &next, types);
			judge(stacked);
		} else {
			moveBlock(&moving, key, stacked);
		}
		printStackedBlock(stacked);
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

void moveBlock(Block* b, int key, int s[WIDTH][HEIGHT]){
	Block cp = *b;
	switch(key){
	case KEY_LEFT:
		cp.pos.x -= 1;
		break;
	case KEY_RIGHT:
		cp.pos.x += 1;
		break;
	case KEY_UP:
		rotateBlock(&cp);
		break;
	default:
		cp.pos.y += 1;
	}
	if(isEnableBlock(cp, s) == 1){
		*b = cp;
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
	//k = rand() % 7;
	k = 0;
	mov->pos.x = LWALL + WIDTH / 2;
        mov->pos.y = LINES / 2 - HEIGHT / 2;
	for(i = 0; i < 4; i++){
		mov->bp[i] = nxt->bp[i];
		nxt->bp[i] = types[k][i];
	}
}

int isEnableBlock(Block b, int s[WIDTH][HEIGHT]){
	int btm = LINES / 2 + HEIGHT / 2;
        int i;
        for(i = 0; i < 4; i++){
					int bX = b.pos.x + b.bp[i].x;
					int bY = b.pos.y + b.bp[i].y;
                if(bY == btm || bX == LWALL || bX == RWALL || s[bX - LWALL][bY - (LINES - HEIGHT) / 2] == 1)
                        return 0;
        }
        return 1;
}

void printStackedBlock(int s[WIDTH][HEIGHT]){
	int i,j;
	for(i = 0; i < WIDTH; i++){
		for(j = 0; j < HEIGHT; j++){
			if(s[i][j] != 0)
				mvaddch(j + (LINES - HEIGHT) / 2, i + LWALL, (i % 2 == 0)?'*':'+');
		}
	}
}

void judge(int s[WIDTH][HEIGHT]){
	int i, j, cnt = 0;
	int completedColumn[HEIGHT] = {};
	for(j = 0; j < HEIGHT; j++){
		i = 0;
		//printf("%d ",s[i][j]);
		while (s[i+1][j] != 0) {
			if(i == WIDTH - 2){
				completedColumn[j] = 1;
				cnt++;
				break;
			}
			i++;
		}
	}
	if(cnt != 0)
		eraceColumn(s, completedColumn, cnt);
}

void eraceColumn(int s[WIDTH][HEIGHT], int e[HEIGHT], int c){
	int i, j, k;
	int *completedColumn = (int *)malloc(sizeof(int) * c);
	j = 0;
	for(i = 0; i < HEIGHT; i++){
		if(e[i] == 1){
			completedColumn[j] = i;
		}
	}
	/*
	for(i = 0; i < 6; i++){
		for(j = 0; j < c; j++){
			for(k = LWALL + 1; k < RWALL; k++){
				mvaddch(completedColumn[j] + (LINES - HEIGHT) / 2, k, (i % 2 == 0) ?' ':'O');
			}
			usleep(WAIT_TIME * 1000);
		}
	}
	*/
	for(i = 0; i < c; i++){
		organizeStacked(s, completedColumn[i]);
	}
	free(completedColumn);
}

void organizeStacked(int s[WIDTH][HEIGHT], int o){
	if(o == 0){
		return;
	}
	int i;
	for(i = 0; i < WIDTH; i++){
		s[i][o] = s[i][o - 1];
	}
	organizeStacked(s,o - 1);
}

void addStackedBlock(int s[WIDTH][HEIGHT], Block b){
	int i;
	for(i = 0; i < 4; i++){
		s[b.pos.x + b.bp[i].x - LWALL][b.pos.y + b.bp[i].y - (LINES - HEIGHT) / 2] = 1;
	}
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
