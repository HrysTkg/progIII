#define WIDTH 50
#define HEIGHT 30
#define LWALL 2
#define RWALL (LWALL + WIDTH)

#define TYPE1 ({{-2,0},{-1.0},{0,0},{1,0}})
#define TYPE2 ({{-1,0},{0,0},{-1,1},{0,1}})
#define TYPE3 ({{-1,0},{0,0},{1,0},{-1,1}})
#define TYPE4 ({{-1,0},{0,0},{1,0},{1,1}})
#define TYPE5 ({{-1,0},{0,0},{1,0},{0,1}})
#define TYPE6 ({{-1,1},{0,1},{0,0},{1,0}})
#define TYPE7 ({{-1,0},{0,0},{0,1},{1,1}})

//Escキー
#define ESC 0x1b

typedef struct{
	int x;
	int y;
}Point;

typedef struct{
	Point bp[4];	//構成ブロック群の座標
	Point pos;	//中心座標
	int life;
}Block;
