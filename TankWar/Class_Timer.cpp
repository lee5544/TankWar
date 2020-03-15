#include "Class_Timer.h"

//来自于网上的代码
LARGE_INTEGER Class_Timer::m_clk = { 0 };
LONGLONG Class_Timer::m_oldclk = 0;
int Class_Timer::m_freq = 0;

//根据需求新增：
LONGLONG Class_Timer::sys_clk = 0;
LONGLONG Class_Timer::game_clk = 0;

//来自于网上的代码
// 延时
void Class_Timer::Sleep(int ms)
{
	if (m_oldclk == 0)
	{
		//QueryPerformanceFrequency(&m_clk);
		//m_freq = (int)m_clk.QuadPart / 1000;	// 获得计数器的时钟频率

		// 开始计时
		QueryPerformanceCounter(&m_clk);
		m_oldclk = m_clk.QuadPart;				// 获得开始时钟
	}

	unsigned int c = ms * m_freq;

	m_oldclk += c;

	QueryPerformanceCounter(&m_clk);

	if (m_clk.QuadPart > m_oldclk)//如果已经到延迟结束的时间
		m_oldclk = m_clk.QuadPart;
	else
		do
		{
			//延时
			::Sleep(1);
			QueryPerformanceCounter(&m_clk);	// 获得终止时钟
		} while (m_clk.QuadPart < m_oldclk);
}

//根据需求新增：
void Class_Timer::initGameTimer()
{
	QueryPerformanceFrequency(&m_clk);
	m_freq = (int)m_clk.QuadPart / 1000;	// 获得计数器的时钟频率
	renewSysClk();
}

void Class_Timer::GameSleep(int ms)
{
	Sleep(ms);
	//刷新一次游戏实际经过的周期数
	QueryPerformanceCounter(&m_clk);
	game_clk += m_clk.QuadPart - sys_clk;
	//延时后刷新一次系统周期数，便于游戏循环执行后进行时间统计
	renewSysClk();
}

DWORD Class_Timer::GetGameTime()
{
	return (DWORD)game_clk / m_freq;
}

void Class_Timer::renewSysClk()
{
	//获取当前系统经过的周期数
	QueryPerformanceCounter(&m_clk);
	sys_clk = m_clk.QuadPart;
}
