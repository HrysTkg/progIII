#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "tetris.h"

/* ゲーム実行関数 */
void run(){
	int key;				//キー入力受け取り用の変数
	Block moving, next;			//移動ブロックと次ブロック
	int blockInitX = LWALL + WIDTH / 2;	//落下ブロックの初期x座標
	int blockInitY = LINES / 2 - HEIGHT / 2;//落下ブロックの初期y座標
	//次のブロックは画面上固定なので、位置をここで指定しておく
	next.pos.x = RWALL + 10;
	next.pos.y = LINES / 2;

	//ブロックの種類を設定するための配列
	Point types[7][4] = {TYPE1,TYPE2,TYPE3,TYPE4,TYPE5,TYPE6,TYPE7};
	int i,j,k;				//ループ変数

	timeout(WAIT_TIME);			//キー入力のタイムアウトをWAIT_TIMEミリ秒に設定する
	srand((unsigned int)time(NULL));	//乱数のシードに現在の時刻を使用

//ゲームスタート画面
GAME_START:
	printTitle();
	for(;;){
		key = getch();
		//キーがs→ゲームスタート,r→ランキング表示,Escキー→終了
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
//ゲームの初期化 スコアをリセットし,積まれたブロックを全て消す
GAME_INIT:
	score = 0;
	for(i = 0; i < WIDTH; i++)
		for(j = 0; j < HEIGHT; j++)
			stacked[i][j] = 0;
//ゲーム本体部
GAME:
	//移動ブロックと次ブロックを決定するため,2回nextBlockを呼び出す
	nextBlock(&moving, &next, types);
	nextBlock(&moving, &next, types);
	//メインループ(Escキーで抜ける)
	while((key = getch()) != ESC){
		clear();
		printStage();

		Block cp = moving;	//移動ブロックのコピーを作成する
		cp.pos.y += 1;		//コピーを1つ下へ動かしてみる
		//コピーが最下地点または他ブロックに衝突しているか調べる
		if(isEnableBlock(cp) == 0){
			addStackedBlock(moving);	//コピーが衝突していた場合 移動ブロックはそこで停止し,積む
			judge();
			if(isGameOver())
				goto GAME_OVER;		//ゲームオーバーなら終了画面へ遷移
			nextBlock(&moving, &next, types);//次のブロックを生成
			score += 10;			//ブロックが1つつまれると10点加算
		} else {
			moveBlock(&moving, key);	//コピーが衝突していなかったら移動ブロックを動かす
		}
		//以下各描画処理
		printStackedBlock();
		printBlock(moving);
		printBlock(next);
	}
	if(key == ESC) goto END;	//Escキーが入力されていたなら終了
//ゲームオーバー部
GAME_OVER:
	//積まれたブロックを点滅させる
	for(i = 0; i < 11; i++){
		(i % 2 == 0)?clearStackedBlock():printStackedBlock();
		refresh();
		usleep(WAIT_TIME * 1000);
	}

	//以下ゲームオーバーのメッセージを表示
	clear();
	mvprintw(LINES/2    , COLS/2 - 5,"GAME OVER");
	mvprintw(LINES/2 + 2, COLS/2 - 15,"Do you want to join Ranking? score:%d",score);
	mvprintw(LINES/2 + 4, COLS/2 - 9,"[y] : Yes , [n] : No");
	//キー入力ループ yキー→スコアを保存 nキー→スコアを保存しない Escキー→終了
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
	goto GAME_START;	//再びゲームスタート画面へ戻る
//ゲーム終了部
END:
	return;
}

/* ステージ描画関数 */
void printStage(){
	int i;
	int j = 0;
	//ステージの床を天井を-=-=で描画
	for(i = LWALL; i <= RWALL; i++){
		mvaddch(LINES / 2 - HEIGHT / 2 - 1, i, (j % 2 == 0)?'-':'=');
		mvaddch(LINES / 2 + HEIGHT / 2, i, (j++ % 2 == 0)?'-':'=');
	}
	//ステージの左右壁と|で描画
	for(i = LINES / 2 - HEIGHT / 2; i < LINES / 2 + HEIGHT / 2; i++){
   		mvaddch(i, LWALL, '|');
		mvaddch(i, RWALL, '|');
	}
	//次のブロックの表示区画とスコアを表示
	mvprintw(LINES / 2 - 2, RWALL + 5,"Next Block");
	mvprintw(LINES / 2 - 1, RWALL + 5,"|        |");
	mvprintw(LINES / 2    , RWALL + 5,"|        |");
	mvprintw(LINES / 2 + 1, RWALL + 5,"|        |");
	mvprintw(LINES / 2 + 2, RWALL + 5,"|        |");
	mvprintw(LINES / 2 + 3, RWALL + 5,"-=-=-=-=-=");
	mvprintw(LINES / 2 + 4, RWALL + 5, "Score:%d",score);
}

/* スタート画面のタイトルを表示する関数 */
void printTitle(){
	mvprintw(LINES/2-3,COLS/2 - 7,"WELCOME TO TETRIS");
	mvprintw(LINES/2  ,COLS/2 - 5,"[s] : Start");
	mvprintw(LINES/2+2,COLS/2 - 5,"[r] : Ranking");
	mvprintw(LINES/2+4,COLS/2 - 5,"[Esc] : Exit");
}

/* 引数のブロックbを@で表示する関数 */
void printBlock(Block b){
	int i;
	for(i = 0; i < 4; i++){
		//ブロックの中心座標posとブロックを構成する各要素の差分bpの和で@を表示させる位置を計算できる
		mvaddch(b.pos.y + b.bp[i].y, b.pos.x + b.bp[i].x, '@');
	}
}

/* 引数のブロックbをkeyの値によって移動させる関数 */
void moveBlock(Block* b, int key){
	//bのコピーを作成し,コピーに対して処理を施す
	Block cp = *b;
	switch(key){
	case KEY_LEFT:			//左へ移動
		cp.pos.x -= 1;
		break;
	case KEY_RIGHT:			//右へ移動
		cp.pos.x += 1;
		break;
	case KEY_UP:			//回転
		rotateBlock(&cp);
		break;
	default:			//キー操作が無かったら単純落下
		cp.pos.y += 1;
	}
	//コピーが有効(他の要素と干渉していない)か確認し,有効ならbをコピーで更新
	if(isEnableBlock(cp) == 1){
		*b = cp;
	}
}

/* 引数のブロックbを回転させる関数 */
void rotateBlock(Block* b){
	int i;
	//回転は回転行列に基づく 一般的な回転行列にθ=-π/2を代入すれば明らか
	for(i = 0; i < 4; i++){
		int temp = b->bp[i].x;
		b->bp[i].x = b->bp[i].y;
		b->bp[i].y = -1 * temp;
	}
}

/* 次ブロックを更新し移動ブロックを前の次ブロックで更新する関数 */
void nextBlock(Block* mov, Block* nxt, Point types[7][4]){
	int i,k;
	k = rand() % 7;				//次ブロックを乱数で決定する
	mov->pos.x = LWALL + WIDTH / 2;		//移動ブロックの中心座標を初期化
        mov->pos.y = LINES / 2 - HEIGHT / 2;
	for(i = 0; i < 4; i++){
		mov->bp[i] = nxt->bp[i];	//移動ブロックを次ブロックで更新
		nxt->bp[i] = types[k][i];	//次ブロックの要素差分を更新
	}
}

/* 引数のブロックbが有効か調べる関数 */
int isEnableBlock(Block b){
	int btm = LINES / 2 + HEIGHT / 2;	//ステージ床のy座標を取得
	int i;
	//ブロックの各要素について調べる
  	for(i = 0; i < 4; i++){
		int bX = b.pos.x + b.bp[i].x;	//ブロック要素の座標を取得
		int bY = b.pos.y + b.bp[i].y;
		//取得した座標が床・壁・積まれたブロックに重なっていないか調べる
    		if(bY == btm || bX == LWALL || bX == RWALL || stacked[bX - LWALL][bY - (LINES - HEIGHT) / 2] == 1)
      			return 0;		//1つでも重なっていれば0を返却
  	}
  	return 1;	//有効であるので1を返却
}

/* 積まれたブロックを描画する */
void printStackedBlock(){
	int i,j;
	//stacked[x][y]が1ならば,ステージ上の(x,y)地点にはブロックが積まれている
	//ここで,(x,y)はステージ上での座標であるため、実際にブロックを表示するスクリーン座標は別で計算する必要あり
	for(i = 0; i < WIDTH; i++){
		for(j = 0; j < HEIGHT; j++){
			if(stacked[i][j] != 0)
				mvaddch(j + (LINES - HEIGHT) / 2, i + LWALL, (i % 2 == 0)?'*':'+');
		}
	}
}

/* 積まれたブロックの描画を消す関数 */
void clearStackedBlock(){
	int i,j;
	//printStackedBlockの表示部分を空白' 'に変更しただけ
	for(i = 0; i < WIDTH; i++){
		for(j = 0; j < HEIGHT; j++){
			if(stacked[i][j] != 0)
				mvaddch(j + (LINES - HEIGHT) / 2, i + LWALL, ' ');
		}
	}
}

/* 揃った行がないかを判定する */
void judge(){
	int i, j, cnt = 0;			//cntは揃った行をカウントする変数
	int completedColumn[HEIGHT] = {};	//揃った行にフラグを立てるための配列
	for(j = 0; j < HEIGHT; j++){
		i = 0;
		//行を走査し,stackedの行全てが1であるところを探す
		while (stacked[i + 1][j] != 0) {
			if(i == WIDTH - 2){		//行の終点へ到達したなら
				completedColumn[j] = 1;	//対応する行へフラグを立てて
				cnt++;			//カウントし
				break;			//ブレイクして次の行へ
			}
			i++;
		}
	}
	//揃った行が1つでもあれば,eraceColumnでその行を消去する
	if(cnt != 0)
		eraceColumn(completedColumn, cnt);
}

/* 揃った行を消去する関数 引数eはそろった行のフラグ,cは揃った行の数 */
void eraceColumn(int e[HEIGHT], int c){
	int i, j, k;
	int *completedColumn = (int *)malloc(sizeof(int) * c);	//揃った行のy座標を格納する配列をmallocで確保
	j = 0;
	for(i = 0; i < HEIGHT; i++){
		//フラグが立っている行の添え字を保存 個の値がy座標と対応する
		if(e[i] == 1){
			completedColumn[j++] = i;
		}
	}
	printStackedBlock();
	//消去する行を6回点滅させる
	for(i = 0; i < 7; i++){
		for(j = 0; j < c; j++){
			for(k = LWALL + 1; k < RWALL; k++){
				mvaddch(completedColumn[j] + (LINES - HEIGHT) / 2, k, (i % 2 == 0) ?' ':(k % 2 == 0)?'*':'+');
			}
		}
		refresh();
		usleep(WAIT_TIME * 1000);
	}
	//stackedの値を消去後のものに変化させる
	for(i = 0; i < c; i++){
		organizeStacked(completedColumn[i]);
	}
	free(completedColumn);		//確保した配列を解放
}

/* 引数で与えられた行を消去し,ブロックの並びを整える関数 */
void organizeStacked(int cc){
	/* 処理は再帰で行う. 引数が0の場合,処理が完了.
	それ以外は,引数の示す行をその一つ上の行で上書きし,cc - 1を新たに引数として再帰 */
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

/* 引数で与えられたブロックを積む関数(つまりstackedにbを追加する) */
void addStackedBlock(Block b){
	int i;
	//bはスクリーン座標,stacked[x][y]の(x,y)はステージ座標であることに注意
	for(i = 0; i < 4; i++){
		stacked[b.pos.x + b.bp[i].x - LWALL][b.pos.y + b.bp[i].y - (LINES - HEIGHT) / 2] = 1;
	}
}

/* ゲームオーバーかの判定を行う関数 */
int isGameOver(){
	//stackedにステージ天井まで到達したブロックがないかを調べる
	int top = 0;
	int i;
	for(i = 0; i < WIDTH; i++){
		if(stacked[i][top] == 1)
			return 1;	//1つでも該当するブロックがあれば1を返却
	}
	return 0;	//無ければ0を返却
}

/* ランキングをファイルから取得し表示する関数 */
void showRanking(){
	FILE *fp = fopen(RECORD_FILE,"r");	//ファイルをオープン ファイル名はtetris.hでマクロ定義
	Record rcd[10];				//ランキングは10位まで表示させるため,要素数は10とする
	int i, j, k;
	char n[64];				//名前を受け取る変数
	int s;					//スコアを受け取る変数
	
	//以下のループでは,全ての記録を読み込み,適切なrcdの要素へ値を保存していく
	j = 0;					//rcdの有効な要素をカウントする
	while(fscanf(fp,"%s %d",n, &s) != EOF){
		if(j < 10){			//rcdにまだ空きがあるなら
			rcd[j].score = 0;	//その要素を初期化し
			j++;			//有効な要素をカウントアップ
		}
		for(i = 0; i < j; i++){		//有効なrcdを走査する
			if(s > rcd[i].score){	//読み込んだスコアの値の方がrcd[i]より高かった場合,rcd[i]へ読み込んだ記録を挿入
				if(i < 9){	//挿入場所がrcdの最後尾以外の場合,元々あった記録をズラす必要あり
					//挿入場所以降の記録を1つ後ろへズラす
					for(k = j - 1; k >= i; k--){
						strcpy(rcd[k].name, rcd[k - 1].name);
						rcd[k].score = rcd[k - 1].score;
					}
				}
				//rcd[i]へ読み込んだ記録を保存
				strcpy(rcd[i].name, n);
				rcd[i].score = s;
				break;
			}
		}
	}
	
	//以下はrcdを有効範囲まで表示する
	clear();
	for(i = 0; i < j; i++){
		mvprintw(LINES / 2 - j + 2*i, (COLS - 10) / 2, "No.%d %s : %dpoints", i + 1,rcd[i].name, rcd[i].score);
	}
	mvprintw(LINES / 2 + j + 2,(COLS - 10)/2,"[ESC] : Exit");
	
	while(getch() != ESC){}			//Escキーが押されるまで無限ループ
	
	clear();
	fclose(fp);				//ファイルクローズ
}

/* 現在のスコアをファイルに保存する関数 */
void saveScore(){
	FILE *fp = fopen(RECORD_FILE, "a");	//ファイルをオープン ファイル名はtetris.hでマクロ定義
	clear();
	//ランキング登録名を入力するための処理
	mvprintw(LINES/2,(COLS-10)/2,"Enter your name");
	move(LINES/2 + 2,(COLS-10)/2);		//カーソルを適当な位置に移動させる
	refresh();				//画面のリフレッシュを行い,画面表示を行う
	echo();					//入力キーを表示させる設定に変更
	char name[64];				//登録名を受け取る変数
	timeout(-1);				//入力のタイムアウトを無くす
	getstr(name);				//入力された情報を受け取る
	fprintf(fp, "%s %d\n",name, score);	//ファイルに登録名とスコアを書式に従って書き込む
	noecho();				//入力キーを表示させない設定に戻す
	clear();				//画面クリア
	timeout(WAIT_TIME);			//タイムアウトを再設定
	fclose(fp);				//ファイルクローズ
}

int main(void){
	initscr();				//cursesを初期化
	noecho();				//入力キーを非表示に設定
	curs_set(0);				//カーソルを非表示に設定
	cbreak();				//入力キーを即時利用に設定
	keypad(stdscr,TRUE);			//キーパッドを設定
	run();					//ゲーム実行
	endwin();				//curses終了処理
	return 0;
}
