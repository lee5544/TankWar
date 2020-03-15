#pragma once
#include <vector>
#include "GameSettings.h"
#include "Class_GamePic.h"
#include "Class_Player.h"
#include "Class_Map.h"
#include "Class_Bullet.h"

using std::vector;

//const unsigned int max_player_bullets = 2;//玩家同时发射的子弹数量上限
//const unsigned int normal_player_bullets = 1;
const unsigned int max_player_bullets = 6;//玩家同时发射的子弹数量上限（翻倍畅玩版）
const unsigned int normal_player_bullets = 3;

class Class_GameWindow
{
private:
	Class_GamePic pictures;//图形对象，管理所有的图片绘制
	Class_Map map;//地图
	Class_Player p1;
	vector<Class_Bullet> bullets;//保存所有的子弹
	unsigned int p1_bullet_count;//记录1P已经发射的炮弹数

public:
	Class_GameWindow();
	//开始游戏
	void play();

protected:
	//刷新画面，effects代表是否使用牛B闪闪特效
	void renewPic(bool effects = true);
	//刷新子弹状态
	void renewBullets();
	//控制函数，传入其它可移动单位，也能让玩家进行控制
	void ctrl(Class_Unit& unit, Class_Map& map);
	//射击
	void shoot(const Class_Unit& tank);
	//改变地形
	void changeMap(const Class_Bullet& bullet);
	void renewStartPic();//刷新游戏开始界面
};

