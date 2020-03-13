#pragma once
#include "Class_Unit.h"

enum MapLayer//每块砖被看成两层
{
	FirstLayer, SecondLayer, LayerCount
};
enum MapIndex//炮弹会击中两块砖
{
	FirstMap, SecondMap, MapIndexCount
};
enum WallCorner//每块砖被分成四个角落，为了处理钢铁碰撞，高位保持为1
{
	Wall_UL = 0xF1, Wall_UR = 0xF2, Wall_DL = 0xF4, Wall_DR = 0xF8
};

class Class_Bullet :
	public Class_Unit
{
public:
	Class_Bullet(const Class_Unit& tank);

	//子弹向前移动一步，如果发生地形碰撞，返回true
	virtual bool move(Direction dir, const Class_Map& map);
	//获取移动速度
	virtual float GetSpeed()const;
	//判断子弹是否发生碰撞，false代表无碰撞，true表示碰撞
	virtual bool ifTouch(const Class_Map& map);
	//获取子弹的所有者
	unsigned int GetOwner()const;

	/*****************
	碰撞检查相关的接口
	*****************/
	//获取检查点坐标
	const Pos_RC (*GetCheckPointsPos()const)[MapIndexCount];
	//获取检查点的值
	const MapInt(*GetCheckPointsVal()const)[MapIndexCount];
	//获取地形碰撞标记
	const bool(*GetTouchFlags()const)[LayerCount][MapIndexCount];
	//获取爆炸贴图的坐标
	virtual const Pos_XY GetBoomXYPos()const;

protected:
	//刷新检查点
	void renewCheckPoints(const Class_Map& map);
	//刷新检查点坐标
	void renewCheckPointsPos();
	//刷新检查点数值
	void renewCheckPointsVal(const Class_Map& map);
	//刷新碰撞检查标记
	void renewTouchFlags();

private:
	UnitType owner;//用来区分每一颗炮弹是谁发射的
	Pos_RC check_points_pos[MapIndexCount];//保存检查点的坐标
	MapInt check_points_val[MapIndexCount];//保存检查点的地图数据
	bool touch_flags[LayerCount][MapIndexCount];//地形碰撞标记（炮弹会碰撞两个地图单位，每个地图单位只碰撞一个角落）
};
