#include "Class_Unit.h"


Class_Unit::Class_Unit(Pos_RC map_pos, UnitType type, Direction dir)
	:map_pos(map_pos), type(type), direction(dir), Armor_Level(NORMAL)
{
	unit_pos.row = map_pos.row * 2;
	unit_pos.col = map_pos.col * 2;
	px_pos.x = (float)map_pos.col * map_px;
	px_pos.y = (float)map_pos.row * map_px;
	end_px_pos = px_pos;
}
Class_Unit::~Class_Unit()
{
}

/********************
控制函数（单位移动等）
********************/
bool Class_Unit::move(Direction dir, const Class_Map& map)
{
	//默认体积碰撞规则为坦克大小
	//获取该单位当前数据
	Pos_RC unit_pos = GetUnitPos();
	Pos_RC map_pos = GetMapPos();
	Direction curDir = GetDirection();

	//90度转向时的调整，对齐地图坐标
	if ((dir - curDir) % 2)
	{
		switch (curDir)
		{
		case UP:
			if (unit_pos.row % 2)
			{
				unit_pos.row--;
			}
			break;
		case LEFT:
			if (unit_pos.col % 2)
			{
				unit_pos.col--;
			}
			break;
		case DOWN:
			if (unit_pos.row % 2)
			{
				unit_pos.row++;
			}
			break;
		case RIGHT:
			if (unit_pos.col % 2)
			{
				unit_pos.col++;
			}
			break;
		default:
			break;
		}
		map_pos.row = unit_pos.row / 2;
		map_pos.col = unit_pos.col / 2;
		SetUnitPos(unit_pos);
		SetMapPos(map_pos);
		SetXYPos(GetEndXYPos());
	}

	//根据移动方向修正一次map_pos
	switch (dir)
	{
	case UP:
		//修正半格地图对齐的碰撞监测点
		map_pos.row = unit_pos.row / 2 + unit_pos.row % 2;
		break;
	case LEFT:
		//修正半格地图对齐的碰撞监测点
		map_pos.col = unit_pos.col / 2 + unit_pos.col % 2;
		break;
	case DOWN:
		map_pos.row = unit_pos.row / 2;
		break;
	case RIGHT:
		map_pos.col = unit_pos.col / 2;
		break;
	default:
		break;
	}
	SetMapPos(map_pos);

	//开始移动
	switch (dir)
	{
	case UP:
		SetDirection(UP);//修改坦克朝向
		if (!ifTouch(map))//如果没有发生碰撞
		{
			unit_pos.row--;
		}
		break;
	case LEFT:
		SetDirection(LEFT);
		if (!ifTouch(map))//如果没有发生碰撞
		{
			unit_pos.col--;
		}
		break;
	case DOWN:
		SetDirection(DOWN);
		if (!ifTouch(map))//如果没有发生碰撞
		{
			unit_pos.row++;
		}
		break;
	case RIGHT:
		SetDirection(RIGHT);
		if (!ifTouch(map))//如果没有发生碰撞
		{
			unit_pos.col++;
		}
		break;
	default:
		break;
	}
	if (unit_pos == GetUnitPos())
	{
		//如果移动代码运行后，坐标和之前相等，说明发生了碰撞
		return true;
	}
	SetUnitPos(unit_pos);

	//刷新一次map_pos，便于下一次移动时的碰撞检测
	switch (GetDirection())
	{
	case UP:
		//修正半格地图对齐的碰撞监测点
		map_pos.row = unit_pos.row / 2 + unit_pos.row % 2;
		break;
	case LEFT:
		//修正半格地图对齐的碰撞监测点
		map_pos.col = unit_pos.col / 2 + unit_pos.col % 2;
		break;
	case DOWN:
		map_pos.row = unit_pos.row / 2;
		break;
	case RIGHT:
		map_pos.col = unit_pos.col / 2;
		break;
	default:
		break;
	}
	SetMapPos(map_pos);
	return false;

}
bool Class_Unit::renewXYPos()
{
	Pos_XY cur_pos = GetXYPos();
	Pos_XY end_pos = GetEndXYPos();
	float speed = GetSpeed();

	//根据方向和终点绘图坐标，判断是否刷新当前绘图坐标
	switch (GetDirection())
	{
	case UP:
		if (cur_pos.y > end_pos.y)
		{
			cur_pos.y -= speed;
		}
		break;
	case LEFT:
		if (cur_pos.x > end_pos.x)
		{
			cur_pos.x -= speed;
		}
		break;
	case DOWN:
		if (cur_pos.y < end_pos.y)
		{
			cur_pos.y += speed;
		}
		break;
	case RIGHT:
		if (cur_pos.x < end_pos.x)
		{
			cur_pos.x += speed;
		}
		break;
	default:
		break;
	}
	SetXYPos(cur_pos);//刷新绘图坐标

	//根据情况确定返回值
	switch (GetDirection())
	{
	case UP:
		if (cur_pos.y <= end_pos.y)
		{
			return true;
		}
		break;
	case LEFT:
		if (cur_pos.x <= end_pos.x)
		{
			return true;
		}
		break;
	case DOWN:
		if (cur_pos.y >= end_pos.y)
		{
			return true;
		}
		break;
	case RIGHT:
		if (cur_pos.x >= end_pos.x)
		{
			return true;
		}
		break;
	default:
		break;
	}

	return false;
}
bool Class_Unit::ifTouch(const Class_Map& map) const
{
	Pos_RC curPos[2] = { GetMapPos(),GetMapPos() };//获取当前行列坐标，有两个碰撞判定点
	//计算需要进行体积碰撞判定的两个点
	switch (GetDirection())
	{
	case UP:
		curPos[0].row--;
		curPos[1].row--;
		curPos[1].col++;
		break;
	case LEFT:
		curPos[0].col--;
		curPos[1].col--;
		curPos[1].row++;
		break;
	case DOWN:
		curPos[0].row += 2;
		curPos[1].row += 2;
		curPos[1].col++;
		break;
	case RIGHT:
		curPos[0].col += 2;
		curPos[1].col += 2;
		curPos[1].row++;
		break;
	default:
		break;
	}

	//碰撞判断
	for (size_t i = 0; i < 2; i++)
	{
		MapInt checkPoint = map.GetVal(curPos[i]);//获取需要检查的地图点取值
		if (checkPoint > EMPTY&& checkPoint <= SEA)
		{
			return true;//发生碰撞
		}
	}
	return false;//未发生碰撞
}

/***********
Set系列函数
***********/
void Class_Unit::SetXYPos(const Pos_XY& px_pos)
{
	this->px_pos = px_pos;
}
void Class_Unit::SetEndXYPos(const Pos_XY& end_px_pos)
{
	this->end_px_pos = end_px_pos;
}
void Class_Unit::SetMapPos(const Pos_RC& map_pos)
{
	this->map_pos = map_pos;
}
void Class_Unit::SetUnitPos(const Pos_RC& unit_pos)
{
	this->unit_pos = unit_pos;
	//更新绘图终点
	end_px_pos.x = (float)unit_pos.col * half_map_px;
	end_px_pos.y = (float)unit_pos.row * half_map_px;

	//px_pos = end_px_pos;//测试用语句，补帧完成后删除
}
void Class_Unit::SetDirection(Direction newDirection)
{
	direction = newDirection;
}
void Class_Unit::SetArmorLev(Armor newArmorLev)
{
	Armor_Level = newArmorLev;
}

/**********
Get系列函数
**********/
const Pos_XY& Class_Unit::GetXYPos()const
{
	return px_pos;
}
const Pos_XY& Class_Unit::GetEndXYPos() const
{
	return end_px_pos;
}
const UnitType& Class_Unit::GetType()const
{
	return type;
}
const Pos_RC& Class_Unit::GetMapPos() const
{
	return map_pos;
}
const Pos_RC& Class_Unit::GetUnitPos() const
{
	return unit_pos;
}
const Direction& Class_Unit::GetDirection()const
{
	return direction;
}
const Armor& Class_Unit::GetArmorLev()const
{
	return Armor_Level;
}
float Class_Unit::GetSpeed() const
{
	//默认基类返回0，具体取值由各继承类完成
	return 0;
}