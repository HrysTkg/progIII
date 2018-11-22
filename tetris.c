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
		mvaddch(LINES / 2 - HEIGHT / 2 - 1, i, (j % 2 == 0)?'-':'=');
		mvaddch(LINES / 2 + HEIGHT / 2, i, (j++ % 2 == 0)?'-':'=');
	}
	for(i = LINES / 2 - HEIGHT / 2; i < LINES / 2 + HEIGHT / 2; i++){
   	mvaddch(i, LWALL, '|');
    mvaddch(i, RWALL, '|');
	}
	mvprintw(LINES / 2 - 2, RWALL + 5,"Next Block");
	mvprintw(LINES / 2 - 1, RWALL + 5,"|        |");
	mvprintw(LINES / 2    , RWALL + 5,"|        |");
	mvprintw(LINES / 2 + 1, RWALL + 5,"|        |");
	mvprintw(LINES / 2 + 2, RWALL + 5,"|        |");
	mvprintw(LINES / 2 + 3, RWALL + 5,"-=-=-=-=-=");
	mvprintw(LINES / 2 + 4, RWALL + 5, "Score:%d",score);
}

void printTitle(){
	mvprintw(LINES/2-3,COLS/2 - 7,"WELCOME TO TETRIS");
	mvprintw(LINES/2  ,COLS/2 - 5,"[s] : Start");
	mvprintw(LINES/2+2,COLS/2 - 5,"[r] : Ranking");
	mvprintw(LINES/2+4,COLS/2 - 5,"[Esc] : Exit");
}

void run(){
	int key;	//キー入力受け取り用の変数

	Block moving, next;	//動いているブロックと次のブロック
	int blockInitX = LWALL + WIDTH / 2;
	int blockInitY = LINES / 2 - HEIGHT / 2;
	next.pos.x = RWALL + 10;
	next.pos.y = LINES / 2;

	Point types[7][4] = {TYPE1,TYPE2,TYPE3,TYPE4,TYPE5,TYPE6,TYPE7};
	int i,j,k;

	timeout(WAIT_TIME);
	srand((unsigned int)time(NULL));
	k = rand() % 7;

GAME_START:
	printTitle();
	for(;;){
		key = getch();
		if(key == 's'){
			clear();
			goto GAME_INIT;
		}else if(key == 'r'){
			showRanking();
			printTitle();
		}else if(key == ESC){
			return;
		}
	}
GAME_INIT:
	score = 0;
	for(i = 0; i < WIDTH; i++)
		for(j = 0; j < HEIGHT; j++)
			stacked[i][j] = 0;
GAME:
	nextBlock(&moving, &next, types);
	nextBlock(&moving, &next, types);
	//メインループ
	while((key = getch()) != ESC){
		clear();
		printStage();
		Block cp = moving;
		cp.pos.y += 1;
		if(isEnableBlock(cp) == 0){
			addStackedBlock(moving);
			judge();
			if(isGameOver())
				goto GAME_OVER;
			nextBlock(&moving, &next, types);
			score += 10;
		} else {
			moveBlock(&moving, key);
		}
		printStackedBlock();
		printBlock(moving);
		printBlock(next);
	}
	if(key == ESC) goto END;
GAME_OVER:
	for(i = 0; i < 11; i++){
		(i % 2 == 0)?clearStackedBlock():printStackedBlock();
		refresh();
		usleep(WAIT_TIME * 1000);
	}
	clear();
	mvprintw(LINES/2    , COLS/2 - 5,"GAME OVER");
	mvprintw(LINES/2 + 2, COLS/2 - 15,"Do you want to join Ranking? score:%d",score);
	mvprintw(LINES/2 + 4, COLS/2 - 9,"[y] : Yes , [n] : No");
	while((key = getch()) != ESC){
		if(key == 'y'){
			saveScore();
			break;
		}
		else if(key == 'n')
			break;
		else if(key == ESC)
			goto END;
	}
	clear();
	goto GAME_START;
END:
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
	if(isEnableBlock(cp) == 1){
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
	k = rand() % 7;
	//k = 0;
	mov->pos.x = LWALL + WIDTH / 2;
        mov->pos.y = LINES / 2 - HEIGHT / 2;
	for(i = 0; i < 4; i++){
		mov->bp[i] = nxt->bp[i];
		nxt->bp[i] = types[k][i];
	}
}

int isEnableBlock(Block b){
	int btm = LINES / 2 + HEIGHT / 2;
  int i;
  for(i = 0; i < 4; i++){
		int bX = b.pos.x + b.bp[i].x;
		int bY = b.pos.y + b.bp[i].y;
    if(bY == btm || bX == LWALL || bX == RWALL || stacked[bX - LWALL][bY - (LINES - HEIGHT) / 2] == 1)
      return 0;
  }
  return 1;
}

void printStackedBlock(){
	int i,j;
	for(i = 0; i < WIDTH; i++){
		for(j = 0; j < HEIGHT; j++){
			if(stacked[i][j] != 0)
				mvaddch(j + (LINES - HEIGHT) / 2, i + LWALL, (i % 2 == 0)?'*':'+');
		}
	}
}

void clearStackedBlock(){
	int i,j;
	for(i = 0; i < WIDTH; i++){
		for(j = 0; j < HEIGHT; j++){
			if(stacked[i][j] != 0)
				mvaddch(j + (LINES - HEIGHT) / 2, i + LWALL, ' ');
		}
	}
}

void judge(){
	int i, j, cnt = 0;
	int completedColumn[HEIGHT] = {};
	for(j = 0; j < HEIGHT; j++){
		i = 0;
		//printf("%d ",s[i][j]);
		while (stacked[i+1][j] != 0) {
			if(i == WIDTH - 2){
				completedColumn[j] = 1;
				cnt++;
				break;
			}
			i++;
		}
	}
	if(cnt != 0)
		eraceColumn(completedColumn, cnt);
}

void eraceColumn(int e[HEIGHT], int c){
	int i, j, k;
	int *completedColumn = (int *)malloc(sizeof(int) * c);
	j = 0;
	for(i = 0; i < HEIGHT; i++){
		if(e[i] == 1){
			completedColumn[j++] = i;
		}
	}
	printStackedBlock();
	for(i = 0; i < 7; i++){
		for(j = 0; j < c; j++){
			for(k = LWALL + 1; k < RWALL; k++){
				mvaddch(completedColumn[j] + (LINES - HEIGHT) / 2, k, (i % 2 == 0) ?' ':(k % 2 == 0)?'*':'+');
			}
		}
		refresh();
		usleep(WAIT_TIME * 1000);
	}

	for(i = 0; i < c; i++){
		organizeStacked(completedColumn[i]);
	}
	free(completedColumn);
}

void organizeStacked(int cc){
	if(cc == 0){
		score += 100;
		return;
	}
	int i;
	for(i = 0; i < WIDTH; i++){
		stacked[i][cc] = stacked[i][cc - 1];
	}
	organizeStacked(cc - 1);
}

void addStackedBlock(Block b){
	int i;
	for(i = 0; i < 4; i++){
		stacked[b.pos.x + b.bp[i].x - LWALL][b.pos.y + b.bp[i].y - (LINES - HEIGHT) / 2] = 1;
	}
}

int isGameOver(){
	int top = 0;
	int i;
	for(i = 0; i < WIDTH; i++){
		if(stacked[i][top] == 1)
			return 1;
	}
	return 0;
}

void showRanking(){
	FILE *fp = fopen(RECORD_FILE,"r");
	Record rcd[10];
	int i,j;
	i = 0;
	while(fscanf(fp,"%s %d",rcd[i].name, &rcd[i].score) != EOF){
		if(++i == 10)
			break;
	}
	clear();
	int cnt = i;
	for(i = 0; i < cnt; i++){
		for(j = 0; j < cnt - 1 - i; j++){
			if(rcd[j].score < rcd[j + 1].score){
      				swap(Record, rcd[j], rcd[j + 1]);
			}
  		}
	}
	for(i = 0; i < cnt; i++){
		mvprintw(LINES / 2 - cnt + 2*i, (COLS - 10) / 2, "No.%d %s : %dpoints", i + 1,rcd[i].name, rcd[i].score);
	}
	mvprintw(LINES / 2 + cnt + 2,(COLS - 10)/2,"[ESC] : Exit");
	while(getch() != ESC){}
	clear();
	fclose(fp);
}

void saveScore(){
	FILE *fp = fopen(RECORD_FILE, "a");
	clear();
	mvprintw(LINES/2,(COLS-10)/2,"Enter your name");
	move(LINES/2 + 2,(COLS-10)/2);
	refresh();
	echo();
	char name[64];
	timeout(-1);
	getstr(name);
	fprintf(fp, "%s %d\n",name, score);
	noecho();
	clear();
	timeout(WAIT_TIME);
	fclose(fp);
}

int main(void){
	initscr();
	noecho();
	curs_set(0);
	cbreak();
	keypad(stdscr,TRUE);
	run();
	endwin();
	return 0;
}
