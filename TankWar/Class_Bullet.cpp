#include "Class_Bullet.h"

Class_Bullet::Class_Bullet(const Class_Unit& tank)
	:Class_Unit(tank.GetMapPos(),BULLET,tank.GetDirection()),owner(tank.GetType())
{
	Pos_RC unit_pos = tank.GetUnitPos();//存储修正后的unit坐标
	Pos_XY px_pos = GetXYPos();//存储修正后的绘图坐标
	//修正子弹单位的坐标
	switch (GetDirection())
	{
	case UP:
		unit_pos.col++;
		px_pos.x = unit_pos.col * half_map_px + half_map_px / 2;
		break;
	case LEFT:
		unit_pos.row++;
		px_pos.y = unit_pos.row * half_map_px + half_map_px / 2;
		break;
	case DOWN:
		unit_pos.col++;
		unit_pos.row += 3;
		px_pos.x = unit_pos.col * half_map_px + half_map_px / 2;
		px_pos.y = unit_pos.row * half_map_px + half_map_px / 2;
		break;
	case RIGHT:
		unit_pos.row++;
		unit_pos.col += 3;
		px_pos.x = unit_pos.col * half_map_px + half_map_px / 2;
		px_pos.y = unit_pos.row * half_map_px + half_map_px / 2;
		break;
	default:
		break;
	}
	SetUnitPos(unit_pos);
	SetXYPos(px_pos);
	unit_pos.row /= 2;
	unit_pos.col /= 2;
	SetMapPos(unit_pos);
	SetArmorLev(tank.GetArmorLev());

	for (size_t iLayer = 0; iLayer < LayerCount; iLayer++)
	{
		for (size_t iMap = 0; iMap < MapIndexCount; iMap++)
		{
			touch_flags[iLayer][iMap] = false;
		}
	}
	for (size_t iMap = 0; iMap < MapIndexCount; iMap++)
	{
		check_points_pos[iMap] = GetMapPos();
		check_points_val[iMap] = EMPTY;
	}
}


bool Class_Bullet::move(Direction dir, const Class_Map& map)
{
	//获取子弹当前数据
	Pos_RC unit_pos = GetUnitPos();
	Pos_RC map_pos = GetMapPos();

	//根据移动方向修正一次map_pos
	switch (dir)
	{
	case UP:
		//map_pos.row = unit_pos.row / 2 + unit_pos.row % 2;
		map_pos.row = unit_pos.row / 2;
		break;
	case LEFT:
		//map_pos.col = unit_pos.col / 2 + unit_pos.col % 2;
		map_pos.col = unit_pos.col / 2;
		break;
	case DOWN:
		//map_pos.row = unit_pos.row / 2;
		map_pos.row = unit_pos.row / 2 - unit_pos.row % 2;
		break;
	case RIGHT:
		//map_pos.col = unit_pos.col / 2;
		map_pos.col = unit_pos.col / 2 - unit_pos.col % 2;
		break;
	default:
		break;
	}
	SetMapPos(map_pos);

	//开始移动
	switch (dir)
	{
	case UP:
		//SetDirection(UP);//修改单位朝向
		if (!ifTouch(map))//如果没有发生碰撞
		{
			unit_pos.row--;
		}
		break;
	case LEFT:
		//SetDirection(LEFT);
		if (!ifTouch(map))//如果没有发生碰撞
		{
			unit_pos.col--;
		}
		break;
	case DOWN:
		//SetDirection(DOWN);
		if (!ifTouch(map))//如果没有发生碰撞
		{
			unit_pos.row++;
		}
		break;
	case RIGHT:
		//SetDirection(RIGHT);
		if (!ifTouch(map))//如果没有发生碰撞
		{
			unit_pos.col++;
		}
		break;
	default:
		break;
	}
	if (unit_pos==GetUnitPos())
	{
		//如果移动代码运行后，坐标和之前相等，说明发生了碰撞
		return true;
	}
	SetUnitPos(unit_pos);
	return false;
}

float Class_Bullet::GetSpeed() const
{
	//区分玩家的子弹还是电脑玩家的子弹
	if (owner==CP)
	{
		//电脑玩家的子弹速度
		if (GetArmorLev()==STRONG)
		{
			return VeryHighSpeed / 1000.0f;
		}
		else
		{
			return HighSpeed / 1000.0f;
		}
	}
	else
	{
		//玩家的子弹速度
		if (GetArmorLev()==NORMAL)
		{
			return HighSpeed / 1000.0f;
		}
		else
		{
			return VeryHighSpeed / 1000.0f;
		}
	}
}

bool Class_Bullet::ifTouch(const Class_Map& map)
{
	//bool des_flag[2][2] = { false };
	Direction dir = GetDirection();
	//MapInt map_tmp[2] = { 0 };//保存检查点的地图数据
	//Pos_RC curPos[MapIndexCount] = { GetMapPos(),GetMapPos() };//获取当前行列坐标，有两个碰撞判定点
	//计算需要进行体积碰撞判定的两个点
	renewCheckPoints(map);

	//switch (GetDirection())
	//{
	//case UP:
	//case DOWN:
	//	curPos[1].col++;
	//	break;
	//case RIGHT:
	//case LEFT:
	//	curPos[1].row++;
	//	break;
	//default:
	//	break;
	//}
	////获取检查点的地图值
	//for (size_t i = 0; i < MapIndexCount; i++)
	//{
	//	map_tmp[i] = map.GetVal(curPos[i]);
	//}

	//碰撞判断
	//先检查炮弹是否会击中砖块的角落
	renewTouchFlags();
	//switch (dir)
	//{
	//case UP:
	//	//先检查第一层砖是否会被碰到
	//	if (map_tmp[FirstMap] & Wall_DR)
	//	{
	//		touch_flags[FirstLayer][FirstMap] = true;
	//	}
	//	if (map_tmp[SecondMap] & Wall_DL)
	//	{
	//		touch_flags[FirstLayer][SecondMap] = true;
	//	}
	//	//再检查第二层砖是否会被碰到
	//	if (map_tmp[FirstMap] & Wall_UR)
	//	{
	//		touch_flags[SecondLayer][FirstMap] = true;
	//	}
	//	if (map_tmp[SecondMap] & Wall_UL)
	//	{
	//		touch_flags[SecondLayer][SecondMap] = true;
	//	}
	//	break;
	//case LEFT:
	//	//先检查第一层砖是否会被碰到
	//	if (map_tmp[FirstMap] & Wall_DR)
	//	{
	//		touch_flags[FirstLayer][FirstMap] = true;
	//	}
	//	if (map_tmp[SecondMap] & Wall_UR)
	//	{
	//		touch_flags[FirstLayer][SecondMap] = true;
	//	}
	//	//再检查第二层砖是否会被碰到
	//	if (map_tmp[FirstMap] & Wall_DL)
	//	{
	//		touch_flags[SecondLayer][FirstMap] = true;
	//	}
	//	if (map_tmp[SecondMap] & Wall_UL)
	//	{
	//		touch_flags[SecondLayer][SecondMap] = true;
	//	}
	//	break;
	//case DOWN:
	//	//先检查第一层砖是否会被碰到
	//	if (map_tmp[FirstMap] & Wall_UR)
	//	{
	//		touch_flags[FirstLayer][FirstMap] = true;
	//	}
	//	if (map_tmp[SecondMap] & Wall_UL)
	//	{
	//		touch_flags[FirstLayer][SecondMap] = true;
	//	}
	//	//再检查第二层砖是否会被碰到
	//	if (map_tmp[FirstMap] & Wall_DR)
	//	{
	//		touch_flags[SecondLayer][FirstMap] = true;
	//	}
	//	if (map_tmp[SecondMap] & Wall_DL)
	//	{
	//		touch_flags[SecondLayer][SecondMap] = true;
	//	}
	//	break;
	//case RIGHT:
	//	//先检查第一层砖是否会被碰到
	//	if (map_tmp[FirstMap] & Wall_DL)
	//	{
	//		touch_flags[FirstLayer][FirstMap] = true;
	//	}
	//	if (map_tmp[SecondMap] & Wall_UL)
	//	{
	//		touch_flags[FirstLayer][SecondMap] = true;
	//	}
	//	//再检查第二层砖是否会被碰到
	//	if (map_tmp[FirstMap] & Wall_DR)
	//	{
	//		touch_flags[SecondLayer][FirstMap] = true;
	//	}
	//	if (map_tmp[SecondMap] & Wall_UR)
	//	{
	//		touch_flags[SecondLayer][SecondMap] = true;
	//	}
	//	break;
	//default:
	//	break;
	//}

	//检查体积碰撞
	for (size_t iLayer = 0; iLayer < LayerCount; iLayer++)
	{
		for (size_t iMap = 0; iMap < MapIndexCount; iMap++)
		{
			if (check_points_val[iMap] > EMPTY && check_points_val[iMap] <= BORDER)
			{
				if (touch_flags[iLayer][iMap] == true)
				{
					return true;
				}
			}
		}
	}
	//for (size_t i = 0; i < 2; i++)
	//{
	//	MapInt checkPoint = map.GetVal(curPos[i]);//获取需要检查的地图点取值
	//	if (checkPoint > EMPTY&& checkPoint <= BORDER)
	//	{
	//		return true;//发生碰撞
	//	}
	//}
	return false;//未发生碰撞
}

unsigned int Class_Bullet::GetOwner()const
{
	return owner;
}

const Pos_RC(*Class_Bullet::GetCheckPointsPos() const)[MapIndexCount]
{
	return &check_points_pos;
}

const MapInt(*Class_Bullet::GetCheckPointsVal() const)[MapIndexCount]
{
	return &check_points_val;
}

const bool(*Class_Bullet::GetTouchFlags() const)[LayerCount][MapIndexCount]
{
	return &touch_flags;
}

const Pos_XY Class_Bullet::GetBoomXYPos() const
{
	//根据终点绘图坐标得到一个爆炸贴图的绘图坐标
	Pos_XY tmp = GetEndXYPos();
	tmp.x -= boom_px / 2 - half_map_px;
	tmp.y -= boom_px / 2 - half_map_px;
	//增加一个坐标偏移，让爆炸点不会总是重合在一起
	int r[2] = { 0 };//获取一个坐标偏移量
	bool flag[2] = { false };//用来控制xy的坐标偏移采用加法还是减法
	for (size_t i = 0; i < 2; i++)
	{
		flag[i] = rand() % 2;
		r[i] = rand() % (source_map_px / 2) * px_multiple;
	}
	tmp.x += pow(-1, flag[0]) * r[0];
	tmp.y += pow(-1, flag[1]) * r[1];
	return tmp;
}

void Class_Bullet::renewCheckPoints(const Class_Map& map)
{
	renewCheckPointsPos();
	renewCheckPointsVal(map);
}

void Class_Bullet::renewCheckPointsPos()
{
	//获取当前行列坐标，有两个碰撞判定点
	for (size_t iMap = 0; iMap < MapIndexCount; iMap++)
	{
		check_points_pos[iMap] = GetMapPos();
	}
	//计算需要进行体积碰撞判定的两个点
	switch (GetDirection())
	{
	case UP:
	case DOWN:
		check_points_pos[1].col++;
		break;
	case LEFT:
	case RIGHT:
		check_points_pos[1].row++;
		break;
	default:
		break;
	}
}

void Class_Bullet::renewCheckPointsVal(const Class_Map& map)
{
	//获取检查点的地图值
	for (size_t i = 0; i < MapIndexCount; i++)
	{
		check_points_val[i] = map.GetVal(check_points_pos[i]);
	}
}

void Class_Bullet::renewTouchFlags()
{
	//先检查炮弹是否会击中砖块的角落
	switch (GetDirection())
	{
	case UP:
		//先检查第一层砖是否会被碰到
		if (check_points_val[FirstMap] & Wall_DR)
		{
			touch_flags[FirstLayer][FirstMap] = true;
		}
		if (check_points_val[SecondMap] & Wall_DL)
		{
			touch_flags[FirstLayer][SecondMap] = true;
		}
		//再检查第二层砖是否会被碰到
		if (check_points_val[FirstMap] & Wall_UR)
		{
			touch_flags[SecondLayer][FirstMap] = true;
		}
		if (check_points_val[SecondMap] & Wall_UL)
		{
			touch_flags[SecondLayer][SecondMap] = true;
		}
		break;
	case LEFT:
		//先检查第一层砖是否会被碰到
		if (check_points_val[FirstMap] & Wall_DR)
		{
			touch_flags[FirstLayer][FirstMap] = true;
		}
		if (check_points_val[SecondMap] & Wall_UR)
		{
			touch_flags[FirstLayer][SecondMap] = true;
		}
		//再检查第二层砖是否会被碰到
		if (check_points_val[FirstMap] & Wall_DL)
		{
			touch_flags[SecondLayer][FirstMap] = true;
		}
		if (check_points_val[SecondMap] & Wall_UL)
		{
			touch_flags[SecondLayer][SecondMap] = true;
		}
		break;
	case DOWN:
		//先检查第一层砖是否会被碰到
		if (check_points_val[FirstMap] & Wall_UR)
		{
			touch_flags[FirstLayer][FirstMap] = true;
		}
		if (check_points_val[SecondMap] & Wall_UL)
		{
			touch_flags[FirstLayer][SecondMap] = true;
		}
		//再检查第二层砖是否会被碰到
		if (check_points_val[FirstMap] & Wall_DR)
		{
			touch_flags[SecondLayer][FirstMap] = true;
		}
		if (check_points_val[SecondMap] & Wall_DL)
		{
			touch_flags[SecondLayer][SecondMap] = true;
		}
		break;
	case RIGHT:
		//先检查第一层砖是否会被碰到
		if (check_points_val[FirstMap] & Wall_DL)
		{
			touch_flags[FirstLayer][FirstMap] = true;
		}
		if (check_points_val[SecondMap] & Wall_UL)
		{
			touch_flags[FirstLayer][SecondMap] = true;
		}
		//再检查第二层砖是否会被碰到
		if (check_points_val[FirstMap] & Wall_DR)
		{
			touch_flags[SecondLayer][FirstMap] = true;
		}
		if (check_points_val[SecondMap] & Wall_UR)
		{
			touch_flags[SecondLayer][SecondMap] = true;
		}
		break;
	default:
		break;
	}
}
