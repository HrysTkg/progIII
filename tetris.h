#define WIDTH 20
#define HEIGHT 20
#define LWALL 2
#define RWALL (LWALL + WIDTH)

#define TYPE1 {{-2,0},{-1.0},{0,0},{1,0}}
#define TYPE2 {{1,0},{0,0},{-1,1},{0,1}}
#define TYPE3 {{-1,0},{0,0},{1,0},{-1,1}}
#define TYPE4 {{-1,0},{0,0},{1,0},{1,1}}
#define TYPE5 {{-1,0},{0,0},{1,0},{0,1}}
#define TYPE6 {{-1,1},{0,1},{0,0},{1,0}}
#define TYPE7 {{-1,0},{0,0},{0,1},{1,1}}

//Escキー
#define ESC 0x1b

#define WAIT_TIME 200
#define RECORD_FILE "record.txt"

int stacked[WIDTH][HEIGHT];
int score;

typedef struct{
	int x;
	int y;
}Point;

typedef struct{
	Point bp[4];	//構成ブロック群の座標
	Point pos;	//中心座標
	int life;
}Block;

void printStage();	//ステージを描画する関数
void run();
void printBlock(Block b);	//ブロックを描画する関数
void clearBlock(Block b);	//ブロックを消す関数
void moveBlock(Block* b, int key);	//キー操作もしくは時間経過によりブロックを移動する関数
void rotateBlock(Block* b);	//ブロックを右回りに90度回転させる関数
void nextBlock(Block* mov, Block* nxt, Point types[7][4]);	//次のブロックを決定する関数
int isEnableBlock(Block b);	//ブロックが有効か判断する関数

void printStackedBlock();	//積まれたブロックを描画する関数
void clearStackedBlock();
void judge();	//揃ったブロック行がないか判定する関数
void addStackedBlock(Block b);	//ブロックを積む関数
void eraceColumn(int e[HEIGHT], int c);	//揃ったブロック行を消す
void organizeStacked(int o);	//積まれたブロックの整理
int isGameOver();	//ゲームオーバー判定

void showRanking();
void saveScore();
