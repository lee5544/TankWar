#pragma once
#include "Class_Tank.h"

const Pos_RC P1_born_pos = { 26,10 };
//const Pos_RC P1_born_pos = { 2,10 };//测试用

class Class_Player :
	public Class_Tank
{
public:
	Class_Player(Pos_RC map_px = P1_born_pos, UnitType typ = P1, Direction dir = UP, Armor Lev = NORMAL);

	/**********
	Get系列函数
	**********/
	//获取移动速度
	virtual float GetSpeed()const;
};

