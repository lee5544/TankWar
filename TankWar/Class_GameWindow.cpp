#include "Class_GameWindow.h"

//void HpSleep(int ms);

Class_GameWindow::Class_GameWindow()
	:p1_bullet_count(0)
{
}

void Class_GameWindow::play()
{
	Class_Unit* unit = &p1;//使用指针实现多态调用

	//bullet = new Class_Bullet(*unit);//测试用

	while (true)
	{
		static DWORD pic_timer = timeGetTime();//控制图像刷新时间
		DWORD now = timeGetTime();

		//如果按下ESC，退出游戏
		if (KEY_DOWN(Key_ESC))
		{
			break;
		}

		//如果到了刷新画面的时间，就执行绘制操作
		if (now - pic_timer >= RenewClock)
		{
			pic_timer = now;
			unit = &p1;
			if (unit)
			{
				if (unit->renewXYPos())
				{
					ctrl(*unit, map);
				}
			}

			//测试用
			//unit = bullet;
			//if (unit)
			//{
			//	if (unit->renewXYPos())
			//	{
			//		ctrl(*unit, map);
			//	}
			//}
			renewBullets();
			renewPic(0);//刷新画面
		}
		Sleep(RenewClock / 3);

		//测试语句（恶搞版），内有关卡切换、坦克形态切换
		static DWORD stage_timer = timeGetTime();//控制关卡刷新时间
		now = timeGetTime();
		if (now - stage_timer >= 5000)
		{
			//map.ChangeStage(rand() % 35 + 1);
			unit->SetArmorLev((Armor)(rand() % ArmorCount));
			stage_timer = now;
		}
	}
}

void Class_GameWindow::renewPic(bool effects)
{
	//是否执行特效
	if (effects)
	{
		pictures.renewBkColor();//更新背景色
		pictures.renewEffects();//更新图片素材的颜色
	}

	cleardevice();//清屏
	pictures.drawMap(map.GetAVal());//绘制地图
	if (!bullets.empty())//绘制炮弹
	{
		for (size_t i = 0; i < bullets.size(); i++)
		{
			pictures.drawBullet(bullets[i]);
		}
	}
	pictures.drawTank(p1);//绘制坦克，测试阶段，以后改为循环访问，绘制所有坦克单位
	pictures.drawJungle(map.GetAVal());//绘制丛林
	pictures.drawBooms();//绘制爆炸贴图

	FlushBatchDraw();//显示到屏幕
}

void Class_GameWindow::renewBullets()
{
	if (!bullets.empty())
	{
		for (auto it = bullets.begin(); it != bullets.end();)
		{
			if (it->renewXYPos())//如果补帧完成
			{
				//让子弹移动
				if (it->move(it->GetDirection(), map))//如果移动后有体积碰撞
				{
					if (it->GetOwner() == P1)
					{
						p1_bullet_count--;
					}
					//修改地形、增加爆炸点，并且删除这枚炮弹
					changeMap(*it);
					pictures.addBoomPoint(it->GetBoomXYPos());
					//pictures.addBoomPoint(it->GetBoomXYPos(), true);//测试大型爆炸用
					it = bullets.erase(it);
					continue;
				}
			}
			it++;
		}
	}
}

int Stage = 1;//设置当前关卡为第一关

void Class_GameWindow::ctrl(Class_Unit& unit, Class_Map& map)
{
	UnitType type = unit.GetType();
	if (type == P1 || type == CP)//该条件测试用
	{
		//当有多个按键按下时，坦克不移动（按键锁定，只允许一个方向移动）
		bool key_state[DirectionCount] = { false };//保存当前按键状态
		int count = 0;//保存当前按下按键的个数

		//记录按键状态
		if (KEY_DOWN(Key_DOWN)||KEY_DOWN(KeyDown))
		{
			key_state[DOWN] = true;
		}
		if (KEY_DOWN(Key_UP)||KEY_DOWN(KeyUp))
		{
			key_state[UP] = true;
		}
		if (KEY_DOWN(Key_LEFT)||KEY_DOWN(KeyLeft))
		{
			key_state[LEFT] = true;
		}
		if (KEY_DOWN(Key_RIGHT)||KEY_DOWN(KeyRight))
		{
			key_state[RIGHT] = true;
		}

		//统计有几个键按下
		for (size_t i = 0; i < DirectionCount; i++)
		{
			if (key_state[i])
			{
				count++;
			}
		}

		//如果只有一个键按下，移动坦克
		if (count == 1)
		{
			size_t i;
			for (i = 0; i < DirectionCount; i++)//找出被按下的键对应的方向
			{
				if (key_state[i] == true)
				{
					break;
				}
			}
			unit.move((Direction)i, map);//坦克移动
		}

		//控制炮弹发射
		if (KEY_DOWN(Key_SHOOT)||KEY_DOWN(KeyShoot))
		{
			const int shoot_cd = 100;//射击CD
			static DWORD shoot_timer = timeGetTime() - shoot_cd;
			DWORD now = timeGetTime();
			if (now - shoot_timer >= shoot_cd)
			{
				shoot_timer = now;
				shoot(unit);
			}
		}
		
		//控制切换关卡
		if (KEY_DOWN(KeyChangeStage))
		{
			Sleep(1000);//软件消抖
			Stage++;
			if (Stage <= max_stage)
				map.ChangeStage(Stage);
			else
			{
				map.ChangeStage(1);
				Stage = 1;
			}		
		}
	}
}

void Class_GameWindow::shoot(const Class_Unit& tank)
{
	if (tank.GetType() == P1)
	{
		//玩家坦克，装甲等级低于LIGHT的时候，只能发射一枚炮弹
		if (tank.GetArmorLev() <= LIGHT)
		{
			if (p1_bullet_count >= normal_player_bullets)
			{
				//如果当前发射的炮弹数目已达上限，直接跳出
				return;
			}
		}
		else//更高装甲等级可以发射两枚炮弹
		{
			if (p1_bullet_count == max_player_bullets)
			{
				//如果当前发射的炮弹数目已达上限，直接跳出
				return;
			}
		}
		p1_bullet_count++;//记录炮弹数目
	}

	bullets.push_back(Class_Bullet(tank));//创建一个子弹到容器中
}

void Class_GameWindow::changeMap(const Class_Bullet& bullet)
{
	DestroyLev dLev = NoDestroy;
	unsigned int owner = bullet.GetOwner();
	Armor armorLev = bullet.GetArmorLev();
	Direction dir = bullet.GetDirection();

	const Pos_RC(*check_points_pos)[MapIndexCount] = bullet.GetCheckPointsPos();
	const MapInt(*check_points_val)[MapIndexCount] = bullet.GetCheckPointsVal();
	const bool(*touch_flags)[LayerCount][MapIndexCount] = bullet.GetTouchFlags();

	//Pos_RC map_pos[2] = { bullet.GetMapPos(), bullet.GetMapPos() };//获取两个检查点的坐标
	//MapInt map_tmp[2] = { 0 };//保存检查点的地图数据

	//如果碰撞点是边界，就直接退出这个函数
	if (map.GetVal((*check_points_pos)[0]) == BORDER)
	{
		return;
	}

	//switch (dir)
	//{
	//	//根据方向调整第二个检测点的坐标
	//case UP:
	//case DOWN:
	//	map_pos[1].col++;
	//	break;
	//case LEFT:
	//case RIGHT:
	//	map_pos[1].row++;
	//	break;
	//default:
	//	break;
	//}
	////获取检查点的地图值
	//for (size_t i = 0; i < 2; i++)
	//{
	//	map_tmp[i] = map.GetVal(map_pos[i]);
	//}

	//根据坦克的阵营和火力等级，确定毁灭地形的程度
	if (owner == CP || owner == P1 && armorLev <= STRONG)
	{
		//如果坦克是敌军（或者玩家的火力较低），设置一次可以消除半块砖
		dLev = HalfDestroy;
	}
	else if (owner == P1 && armorLev == HEAVY)
	{
		dLev = AllDestroy;
	}

	////处理地形碰撞
	////for (size_t i = 0; i < 2; i++)
	////{
	////	MapInt map_tmp = map.GetVal(map_pos[i]);
	////	if (bullet.GetOwner() == CP)
	////	{
	////		if (map_tmp > EMPTY&& map_tmp <= WALL)
	////		{
	////			map.DestroyMap(map_pos[i], bullet.GetDirection(), HalfDestroy);
	////		}
	////		//map.DestroyMap(map_pos[i], bullet.GetDirection(), AllDestroy);
	////		//map.SetVal(map_pos[i], 0);
	////	}
	////}

	//if (dLev == HalfDestroy)
	//{
	//	bool des_flag[2][2] = { false };
	//	//先检查炮弹是否会击中砖块的角落
	//	switch (dir)
	//	{
	//	case UP:
	//		//先检查第一层砖是否会被碰到
	//		if (map_tmp[0] & 0xF8)
	//		{
	//			des_flag[0][0] = true;
	//		}
	//		if (map_tmp[1] & 0xF4)
	//		{
	//			des_flag[0][1] = true;
	//		}
	//		//再检查第二层砖是否会被碰到
	//		if (map_tmp[0] & 0xF2)
	//		{
	//			des_flag[1][0] = true;
	//		}
	//		if (map_tmp[1] & 0xF1)
	//		{
	//			des_flag[1][1] = true;
	//		}
	//		break;
	//	case LEFT:
	//		//先检查第一层砖是否会被碰到
	//		if (map_tmp[0] & 0xF8)
	//		{
	//			des_flag[0][0] = true;
	//		}
	//		if (map_tmp[1] & 0xF2)
	//		{
	//			des_flag[0][1] = true;
	//		}
	//		//再检查第二层砖是否会被碰到
	//		if (map_tmp[0] & 0xF4)
	//		{
	//			des_flag[1][0] = true;
	//		}
	//		if (map_tmp[1] & 0xF1)
	//		{
	//			des_flag[1][1] = true;
	//		}
	//		break;
	//	case DOWN:
	//		//先检查第一层砖是否会被碰到
	//		if (map_tmp[0] & 0xF2)
	//		{
	//			des_flag[0][0] = true;
	//		}
	//		if (map_tmp[1] & 0xF1)
	//		{
	//			des_flag[0][1] = true;
	//		}
	//		//再检查第二层砖是否会被碰到
	//		if (map_tmp[0] & 0xF8)
	//		{
	//			des_flag[1][0] = true;
	//		}
	//		if (map_tmp[1] & 0xF4)
	//		{
	//			des_flag[1][1] = true;
	//		}
	//		break;
	//	case RIGHT:
	//		//先检查第一层砖是否会被碰到
	//		if (map_tmp[0] & 0xF4)
	//		{
	//			des_flag[0][0] = true;
	//		}
	//		if (map_tmp[1] & 0xF1)
	//		{
	//			des_flag[0][1] = true;
	//		}
	//		//再检查第二层砖是否会被碰到
	//		if (map_tmp[0] & 0xF8)
	//		{
	//			des_flag[1][0] = true;
	//		}
	//		if (map_tmp[1] & 0xF2)
	//		{
	//			des_flag[1][1] = true;
	//		}
	//		break;
	//	default:
	//		break;
	//	}

	//	bool flag_1 = false;//标记第一层砖是否已经发生碰撞
	//	for (size_t iLayer = 0; iLayer < 2 && !flag_1; iLayer++)//用来区分砖块的两层
	//	{
	//		for (size_t iMap = 0; iMap < 2; iMap++)//用来区分两块砖
	//		{
	//			if (map_tmp[iMap] > EMPTY && map_tmp[iMap] <= BORDER)
	//			{
	//				if (des_flag[iLayer][iMap] == true)
	//				{
	//					flag_1 = true;//如果第一层就已经发生了碰撞，不会检查第二层砖
	//					map.DestroyMap(map_pos[iMap], dir, dLev);
	//				}
	//			}
	//		}
	//	}
	//}
	//else if (dLev == AllDestroy)
	//{
	//	for (size_t i = 0; i < 2; i++)
	//	{
	//		if (map_tmp[i] > EMPTY && map_tmp[i] <= BORDER)
	//		{
	//			map.DestroyMap(map_pos[i], dir, dLev);
	//		}
	//	}
	//}

	//优化后的碰撞处理
	bool flag_1 = false;//标记第一层砖是否已经发生碰撞
	for (size_t iLayer = 0; iLayer < 2 && !flag_1; iLayer++)//用来区分砖块的两层
	{
		for (size_t iMap = 0; iMap < 2; iMap++)//用来区分两块砖
		{
			if ((*check_points_val)[iMap] > EMPTY && (*check_points_val)[iMap] <= BORDER)
			{
				if ((*touch_flags)[iLayer][iMap] == true)
				{
					flag_1 = true;//如果第一层就已经发生了碰撞，不会检查第二层砖
					map.DestroyMap((*check_points_pos)[iMap], dir, dLev);
				}
			}
		}
	}
}

//void HpSleep(int ms)
//{
//	static clock_t oldclock = clock();		// 静态变量，记录上一次 tick
//
//	oldclock += ms * CLOCKS_PER_SEC / 1000;	// 更新 tick
//
//	if (clock() > oldclock)					// 如果已经超时，无需延时
//		oldclock = clock();
//	else
//		while (clock() < oldclock)			// 延时
//			Sleep(1);						// 释放 CPU 控制权，降低 CPU 占用率
//}