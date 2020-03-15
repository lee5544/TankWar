#pragma once
#include "GameSettings.h"

//行列坐标系
typedef struct
{
	Int row, col;
}Pos_RC;
bool operator ==(const Pos_RC& a, const Pos_RC& b);

const unsigned int max_stage = 35;//地图数量
const Pos_RC BossPos = { 26,14 };
enum DestroyLev//破坏地形的程度
{
	NoDestroy,//不破坏
	HalfDestroy,//消除半块砖（对钢铁无效）
	AllDestroy//消除该地图单位
};
enum HalfWall//用于消除砖块，与该部分按位与后，可以保留该部分（对钢铁无效）
{
	HalfWall_EMPTY=0xF0,//按位与后消除整块砖
	HalfWall_UP = 0xF3,//按位与后保留上半部
	HalfWall_LEFT = 0xF5,//按位与后保留左半部
	HalfWall_DOWN = 0xFC,//按位与后保留下半部
	HalfWall_RIGHT = 0xFA//按位与后保留右半部
};

//地图类
class Class_Map
{
public:
	Class_Map();

	//返回地图数组的首地址
	const MapInt(*GetAVal())[map_row][map_col];
	//返回指定地图行列的数值
	MapInt GetVal(const Pos_RC& map_pos)const;
	//修改地图上的数值
	void SetVal(const Pos_RC& map_pos, MapInt val);
	//根据方向处理地形损坏情况
	void DestroyMap(const Pos_RC& map_pos, const Direction& dir, const DestroyLev& dLev);
	//摧毁指挥部
	void DestroyHeadquarters();
	//切换关卡的函数
	void ChangeStage(unsigned int stage);

private:
	MapInt map[map_row][map_col];//保存地图数据的数组
	void loadmap(unsigned int stage = 1);//从文件读取地图数据，stage是关卡数
};

