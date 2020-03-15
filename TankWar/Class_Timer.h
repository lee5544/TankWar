#pragma once
#include <Windows.h>

class Class_Timer
{
private:
	//网上代码已经有的数据成员：
	static LARGE_INTEGER m_clk;			// 保存时钟信息
	static LONGLONG m_oldclk;			// 保存开始时钟和结束时钟
	static int m_freq;					// 时钟频率(时钟时间换算率)，时间差

	//根据游戏需求新增：
	static LONGLONG sys_clk;//游戏循环一开始时需要刷新这个变量
	static LONGLONG game_clk;//实际游戏经过的周期数（暂停时间不进行记录）

public:
	//网上代码已经有的成员函数：
	static void Sleep(int ms);

	/****************
	根据游戏需求新增：
	****************/
	//初始化计时所需的数据0
	static void initGameTimer();
	//游戏专用延迟（参数代表游戏是否处于暂停状态，false表示没暂停）
	static void GameSleep(int ms);
	//获取游戏实际经过的毫秒数
	static DWORD GetGameTime();
	//刷新游戏循环开始前的系统周期数
	static void renewSysClk();
};