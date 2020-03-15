#include <iostream>
#include <fstream>
#include "Class_Map.h"
using std::ifstream;
using std::wstring;
using std::hex;

Class_Map::Class_Map()
{
	loadmap();
}

const MapInt(*Class_Map::GetAVal())[map_row][map_col]
{
	return &map;
}

MapInt Class_Map::GetVal(const Pos_RC& map_pos)const
{
	return map[map_pos.row][map_pos.col];
}

void Class_Map::SetVal(const Pos_RC& map_pos, MapInt val)
{
	map[map_pos.row][map_pos.col] = val;
}

void Class_Map::DestroyMap(const Pos_RC& map_pos, const Direction& dir, const DestroyLev& dLev)
{
	switch (dLev)
	{
	case NoDestroy:
		return;
		break;
	case HalfDestroy:
	{
		MapInt tmp = map[map_pos.row][map_pos.col];//获取该点的地图数据
		bool flag[DirectionCount] = { false };//保存四个方向是否存在砖块的标记

		//找出砖块还残余哪一部分，便于后续的消除判断
		if (tmp & 1)
		{
			flag[UP] = true;
			flag[LEFT] = true;
		}
		tmp = tmp >> 1;
		if (tmp & 1)
		{
			flag[UP] = true;
			flag[RIGHT] = true;
		}
		tmp = tmp >> 1;
		if (tmp & 1)
		{
			flag[DOWN] = true;
			flag[LEFT] = true;
		}
		tmp = tmp >> 1;
		if (tmp & 1)
		{
			flag[DOWN] = true;
			flag[RIGHT] = true;
		}

		//根据射击方向处理砖块的销毁
		switch (dir)
		{
		case UP:
			if (flag[DOWN])
			{
				//如果下半部砖块存在
				map[map_pos.row][map_pos.col] = map[map_pos.row][map_pos.col] & HalfWall_UP;//保留上半部砖块
			}
			else
			{
				//消除整块砖
				map[map_pos.row][map_pos.col] = map[map_pos.row][map_pos.col] & HalfWall_EMPTY;
			}
			break;
		case LEFT:
			if (flag[RIGHT])
			{
				//如果右半部砖块存在
				map[map_pos.row][map_pos.col] = map[map_pos.row][map_pos.col] & HalfWall_LEFT;//保留左半部砖块
			}
			else
			{
				map[map_pos.row][map_pos.col] = map[map_pos.row][map_pos.col] & HalfWall_EMPTY;
			}
			break;
		case DOWN:
			if (flag[UP])
			{
				//如果上半部砖块存在
				map[map_pos.row][map_pos.col] = map[map_pos.row][map_pos.col] & HalfWall_DOWN;//保留下半部砖块
			}
			else
			{
				map[map_pos.row][map_pos.col] = map[map_pos.row][map_pos.col] & HalfWall_EMPTY;
			}
			break;
		case RIGHT:
			if (flag[LEFT])
			{
				//如果左半部砖块存在
				map[map_pos.row][map_pos.col] = map[map_pos.row][map_pos.col] & HalfWall_RIGHT;//保留右半部砖块
			}
			else
			{
				map[map_pos.row][map_pos.col] = map[map_pos.row][map_pos.col] & HalfWall_EMPTY;
			}
			break;
		default:
			break;
		}
	}
	break;
	case AllDestroy:
		SetVal(map_pos, EMPTY);
		break;
	default:
		break;
	}
}

void Class_Map::DestroyHeadquarters()
{
	for (int col = 0; col < 2; col++)
	{
		for (int row = 0; row < 2; row++)
		{
			map[BossPos.row + row][BossPos.col + col] = HEADQUARTERS_UL_FAIL + row + col;
		}
	}
}

void Class_Map::ChangeStage(unsigned int stage)
{
	loadmap(stage);
}

void Class_Map::loadmap(unsigned int stage)
{
	ifstream map_file;//地图文件流
	wstring temp;//保存地图文件名

	// 产生临时文件的文件名
	TCHAR tmp_map[_MAX_PATH];
	::GetTempPath(_MAX_PATH, tmp_map);
	//_tcscat(tmp_map, _T("map.txt"));
	wcscat_s(tmp_map, _T("map.txt"));
	// 将 MAP 资源提取为临时文件
	ExtractResource(tmp_map, _T("MAP"), _T("map"));

	//temp = map_file_path;
	//temp += map_file_name;
	temp = tmp_map;
	map_file.open(temp);
	if (!map_file.is_open())
	{
		printf("地图文件打开失败！\n");
		exit(1);
	}

	//调整文件读取位置，如果玩家玩到35关以后，就从第一关地图开始
	stage -= 1;
	stage = stage % max_stage;
	map_file.seekg(stage * map_row * map_col * 3 + stage);//文件定位到指定字节（CRLF回车换行，所以要加stage）

	int num_temp;//暂存文件读取的整数，
	for (size_t row = 0; row < map_row; row++)
	{
		for (size_t col = 0; col < map_col; col++)
		{
			//map_file >> hex >> map[row][col];//无法正确读取一个整数
			map_file >> hex >> num_temp;
			map[row][col] = num_temp;
		}
	}
	map_file.close();

	// 删除临时文件
	DeleteFile(tmp_map);
}

//其它函数
//运算符重载
bool operator ==(const Pos_RC& a, const Pos_RC& b)
{
	if (a.row == b.row && a.col == b.col)
	{
		return true;
	}
	return false;
}
