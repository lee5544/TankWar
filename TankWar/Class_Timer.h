#pragma once
#include <Windows.h>

class Class_Timer
{
private:
	//���ϴ����Ѿ��е����ݳ�Ա��
	static LARGE_INTEGER m_clk;			// ����ʱ����Ϣ
	static LONGLONG m_oldclk;			// ���濪ʼʱ�Ӻͽ���ʱ��
	static int m_freq;					// ʱ��Ƶ��(ʱ��ʱ�任����)��ʱ���

	//������Ϸ����������
	static LONGLONG sys_clk;//��Ϸѭ��һ��ʼʱ��Ҫˢ���������
	static LONGLONG game_clk;//ʵ����Ϸ����������������ͣʱ�䲻���м�¼��

public:
	//���ϴ����Ѿ��еĳ�Ա������
	static void Sleep(int ms);

	/********������Ϸ��������********/
	static void initGameTimer();//��ʼ����ʱ���������0
	static void GameSleep(int ms);//��Ϸר���ӳ٣�����������Ϸ�Ƿ�����ͣ״̬��false��ʾû��ͣ��
	static DWORD GetGameTime();//��ȡ��Ϸʵ�ʾ����ĺ�����
	static void renewSysClk();//ˢ����Ϸѭ����ʼǰ��ϵͳ������
};