#pragma once
#include <math.h>
#include <graphics.h>
#pragma comment(lib,"Winmm.lib")

//特殊宏定义
#define Int unsigned short
#define MapInt unsigned char
#define KEY_DOWN(VK_NONAME) (GetAsyncKeyState(VK_NONAME) & 0x8000)	//按键监测，参数为键盘的键值，当某按键被按下时返回非0值

//绘图坐标
typedef struct
{
	float x, y;
}Pos_XY;

/*****************
设置窗口的相关参数
*****************/
const Int source_map_px = 8;//原地图单元对应的像素大小
const Int source_unit_px = source_map_px * 2;//每个单位（坦克、道具等）原始素材的像素大小
const Int source_boom_px = source_unit_px * 2;//爆炸贴图的像素宽度
const Int source_half_map_px = source_map_px / 2;//子弹原始素材的像素大小
const Int px_multiple = 2;//图片像素放大倍数
const Int map_px = px_multiple * source_map_px;//每个地图单元的实际像素大小
const Int unit_px = map_px * 2;//坦克、道具的绘图大小
const Int boom_px = unit_px * 2;//实际爆炸贴图大小
const Int half_map_px = map_px / 2;//子弹绘图大小
//地图数组的行、列数
const Int map_row = 30;
const Int map_col = 32;
//单元数组的行、列数，为地图数组的两倍，以此达到坦克等移动“半格”的效果
const Int unit_row = map_row * 2;
const Int unit_col = map_col * 2;

//游戏界面的宽和高（像素）
const Int map_wide = map_px * map_col;
const Int map_height = map_px * map_row;

const Int unit_size = 4;//每个单元在unit矩阵中占用的宽度（坦克长宽为4，炮弹为2）
const Int bullet_size = unit_size / 2;
const Int unit_sizeInMap = unit_size / 2;

/*******************************
物体移动速度、画面刷新速率等设置
*******************************/
const Int FPS = 60;
const Int RenewClock = 1000 / FPS;//画面刷新周期
enum Speed//每次刷新时物体应移动的像素数*1000（不能用整数直接代表像素，否则画面放大倍数会影响实际移动速度）
{
	SlowSpeed = (2 * unit_sizeInMap * map_px) * RenewClock , //每秒走2大格
	NormalSpeed = (int)(3.5 * unit_sizeInMap * map_px) * RenewClock , //每秒走3.5大格
	FastSpeed = (5 * unit_sizeInMap * map_px) * RenewClock ,//每秒走5大格
	HighSpeed = (int)(1.25 * FastSpeed),
	VeryHighSpeed = (int)(2 * HighSpeed)
};

/***************
各单位的相关参数
***************/
enum MapType//地图类型
{
	EMPTY,//空地
	WALL = 0x0F,//砖墙
	IRON,//防爆门
	BORDER,//地图边界
	SEA,//海面
	ICE = 0x21,//冰面
	JUNGLE,//丛林
};
enum UnitType//各单位类型值，用于碰撞检测
{
	//子弹
	BULLET = 500,
	P1 = 300,//1号玩家
	P2 = 400,//2号玩家
	CP = 600,//敌人
	COMMANDER = 0xC8,//指挥官
};
enum Direction//方向，绘制、开炮时用
{
	UP, LEFT, DOWN, RIGHT, DirectionCount
};
enum Armor//坦克装甲等级
{
	DEAD = -1, NORMAL, LIGHT, STRONG, HEAVY, ArmorCount
};
enum Key//键盘控制
{
	Key_UP = 'W', Key_LEFT = 'A', Key_RIGHT = 'D', Key_DOWN = 'S',
	Key_SHOOT = 'J', Key_PAUSE = 'P',
	Key_START = 'F', Key_SELECT = 'R',
	Key_ESC = VK_ESCAPE,

	KeyUp = VK_UP, KeyDown = VK_DOWN,
	KeyLeft = VK_LEFT, KeyRight = VK_RIGHT,
	KeyShoot = VK_SPACE,//空格键发射炮弹
	KeyChangeStage = VK_RETURN//Enter控制切换关卡
};

// 提取指定模块中的资源文件
// 参数：
//		strDstFile:		目标文件名。提取的资源将保存在这里；
//		strResType:		资源类型；
//		strResName:		资源名称；
// 返回值：
//		true: 执行成功；
//		false: 执行失败。
bool ExtractResource(LPCTSTR strDstFile, LPCTSTR strResType, LPCTSTR strResName);