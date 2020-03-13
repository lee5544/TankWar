#pragma once
#include "GameSettings.h"
#include "Class_Map.h"

//游戏单位类，坦克、子弹、道具的基类
class Class_Unit
{
private:
	Pos_XY px_pos;//当前绘图坐标
	Pos_XY end_px_pos;//移动结束时的绘图坐标，主要用来实现地图对齐、判断单位是否移动结束（补动态帧有用）
	UnitType type;//单位类型
	Pos_RC map_pos;//对齐地图的坐标
	Pos_RC unit_pos;//单位移动时用的坐标，行列数为地图行列数的两倍，实现每次移动半格的效果
	Direction direction;//方向
	Armor Armor_Level;//保存当前装甲等级（装甲等级通过拾取道具提升，同时影响坦克的防御、子弹的破坏力）

public:
	Class_Unit(Pos_RC map_pos, UnitType type, Direction dir = UP);
	virtual ~Class_Unit();

	/********************
	控制函数（单位移动等）
	********************/
	//移动函数（发生体积碰撞时返回true）
	virtual bool move(Direction dir, const Class_Map& map) = 0;
	//virtual void move(const Class_Map& map);
	//刷新单位绘图坐标，返回true代表该单位可以继续在地图上移动（主要用来补帧）
	virtual bool renewXYPos();
	//判断单位是否发生碰撞，false代表无碰撞，true表示碰撞
	virtual bool ifTouch(const Class_Map& map)const;

	/**********
	Set系列函数
	**********/
	//设置当前绘图像素点坐标
	void SetXYPos(const Pos_XY& px_pos);
	//设置终点绘图像素点坐标
	void SetEndXYPos(const Pos_XY& end_px_pos);
	//设置单位对齐地图的坐标
	void SetMapPos(const Pos_RC& map_pos);
	//设置另一组更精细的地图坐标
	void SetUnitPos(const Pos_RC& unit_pos);
	//改变方向
	void SetDirection(Direction);
	//设置装甲等级
	void SetArmorLev(Armor newArmorLev);

	/**********
	Get系列函数
	**********/
	//获取当前绘图坐标
	const Pos_XY& GetXYPos()const;
	//获取终点绘图坐标
	const Pos_XY& GetEndXYPos()const;
	//获取爆炸贴图的坐标
	//virtual const Pos_XY GetBoomXYPos()const;
	//获取单位的类型
	const UnitType& GetType()const;
	//获取单位所在的地图坐标
	const Pos_RC& GetMapPos()const;
	//获取另一组更精细的地图坐标（主要用于子弹碰撞和捡道具等）
	const Pos_RC& GetUnitPos()const;
	//获取方向
	const Direction& GetDirection()const;
	//获取装甲等级
	const Armor& GetArmorLev()const;
	//获取移动速度
	virtual float GetSpeed()const;
};
