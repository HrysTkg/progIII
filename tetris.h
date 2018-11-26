#define WIDTH 20		//ステージの幅
#define HEIGHT 20		//ステージの高さ
#define LWALL (COLS/2 - WIDTH)	//ステージ左壁のx座標
#define RWALL (LWALL + WIDTH)	//ステージ右壁のx座標

//ブロック構成要素の差分座標を定義
#define TYPE1 {{-2,0},{-1.0},{0,0},{1,0}}
#define TYPE2 {{1,0},{0,0},{-1,1},{0,1}}
#define TYPE3 {{-1,0},{0,0},{1,0},{-1,1}}
#define TYPE4 {{-1,0},{0,0},{1,0},{1,1}}
#define TYPE5 {{-1,0},{0,0},{1,0},{0,1}}
#define TYPE6 {{-1,1},{0,1},{0,0},{1,0}}
#define TYPE7 {{-1,0},{0,0},{0,1},{1,1}}

#define ESC 0x1b		//Escキーの値

#define WAIT_TIME 200		//待ち時間の定義
#define RECORD_FILE "record.txt"//記録保存ファイルの定義

/* 交換関数のマクロ定義 */
#define swap(type,a,b) {type t=a;a=b;b=t;}

int stacked[WIDTH][HEIGHT];	//積まれたブロックを管理する
int score;			//スコア

//座標構造体
typedef struct{
	int x;
	int y;
}Point;

//ブロック構造体
typedef struct{
	Point bp[4];	//構成ブロック要素の差分座標
	Point pos;	//中心座標
}Block;

//記録を保存する構造体
typedef struct {
	char name[64];
	int score;
}Record;

/* 以下関数宣言 */
void printStage();
void run();
void printBlock(Block b);
void moveBlock(Block* b, int key);
void rotateBlock(Block* b);
void nextBlock(Block* mov, Block* nxt, Point types[7][4]);
int isEnableBlock(Block b);
void printStackedBlock();
void clearStackedBlock();
void judge();
void addStackedBlock(Block b);
void eraceColumn(int e[HEIGHT], int c);
void organizeStacked(int o);
int isGameOver();
void showRanking();
void saveScore();
