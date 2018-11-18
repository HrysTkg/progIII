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
void moveBlock(Block* b, int key, int s[WIDTH][HEIGHT]);	//キー操作もしくは時間経過によりブロックを移動する関数
void rotateBlock(Block* b);	//ブロックを右回りに90度回転させる関数
void nextBlock(Block* mov, Block* nxt, Point types[7][4]);	//次のブロックを決定する関数
int isEnableBlock(Block b, int s[WIDTH][HEIGHT]);	//ブロックが有効か判断する関数

void printStackedBlock(int s[WIDTH][HEIGHT]);	//積まれたブロックを描画する関数
void judge(int s[WIDTH][HEIGHT]);	//揃ったブロック行がないか判定する関数
void addStackedBlock(int s[WIDTH][HEIGHT], Block b);	//ブロックを積む関数
void eraceColumn(int s[WIDTH][HEIGHT], int e[HEIGHT], int c);	//揃ったブロック行を消す
void organizeStacked(int s[WIDTH][HEIGHT], int o);	//積まれたブロックの整理
