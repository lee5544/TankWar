#include "Class_Player.h"

Class_Player::Class_Player(Pos_RC map_px, UnitType typ, Direction dir, Armor Lev)
	: Class_Tank(map_px, typ, dir, Lev)
{
}

/**********
Get系列函数
**********/
//重写Class_Unit基类的GetSpeed函数
float Class_Player::GetSpeed()const
{
	//以下代码可以放到电脑玩家中
	if (GetType() == CP)
	{
		if (GetArmorLev() == LIGHT)
		{
			return FastSpeed / 1000.0f;
		}
		if (GetArmorLev() == HEAVY)
		{
			return SlowSpeed / 1000.0f;
		}
		return NormalSpeed / 1000.0f;
	}
	return NormalSpeed / 1000.0f;
}