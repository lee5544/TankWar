#pragma once
#include "Class_Unit.h"

//坦克基类，向下派生玩家坦克和电脑玩家坦克
class Class_Tank :
	public Class_Unit
{
private:
	bool trackState;//切换履带显示
	DWORD timer_trackState;//记录履带切换时的时间点

public:
	Class_Tank(Pos_RC map_pos, UnitType type, Direction dir = DOWN, Armor Lev = NORMAL);

	/**********Get系列函数**********/
	bool GetTrackState()const;//获取履带状态

	/**********控制函数（坦克行走等）***********/
	virtual bool move(Direction dir, const Class_Map& map);//坦克移动
	void renewTrackState();//切换履带显示，flag表示是否接收到切换信号，true表示需要切换
};

