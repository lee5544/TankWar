#pragma once
#include <vector>
#include "GameSettings.h"
#include "Class_Tank.h"
#include "Class_Map.h"
#include "Class_Bullet.h"

using std::vector;

enum Camp//阵营
{
	CampPlayer, CampComputerPlayer, CampCount
};
enum MapFileNum//图片文件编号
{
	FileNum_wall, FileNum_iron, FileNum_border, FileNum_sea0, FileNum_sea1, FileNum_ice, FileNum_jungle, MapFileCount
};

/***************
爆炸贴图相关参数
***************/
const unsigned int boom_pic_count = 5;//总的爆炸贴图数目
const unsigned int small_boom_count = 3;//小型爆炸需要用到的图片数量
const unsigned int big_boom_count = boom_pic_count;//大型爆炸需要用到的图片数量
const unsigned int avr_pic_time = 60;//平均每张图片的显示时间
const unsigned int small_boom_time = small_boom_count * avr_pic_time;//小型爆炸持续的时间
const unsigned int big_boom_time = big_boom_count * avr_pic_time;//大型爆炸持续的时间
typedef struct//爆炸贴图需要的数据
{
	int pic_count;//保存该爆炸点需要用的图片数目
	int duration;//持续时间
	DWORD add_time;//记录该爆炸点的时间（根据时间选择绘制图片）
	Pos_XY pos;//记录爆炸绘图坐标
}BoomPoint;

enum BossState//BOSS的状态
{
	BossAlive,BossDead,BossStateCount
};

//图片管理类
class Class_GamePic
{
public:
	Class_GamePic();
	~Class_GamePic();

	/******************
	基本贴图绘制接口
	******************/
	//绘制坦克
	void drawTank(const Class_Tank& tank);
	//绘制地图
	void drawMap(const MapInt(*map)[map_row][map_col]);
	//绘制丛林
	void drawJungle(const MapInt(*map)[map_row][map_col]);
	//绘制炮弹
	void drawBullet(Class_Bullet& bullet);
	//绘制爆炸贴图
	void drawBooms();

	/******************
	爆炸贴图相关接口
	******************/
	//新增一个爆炸点，pos是绘图坐标，state：false是小爆炸，true是大爆炸
	void addBoomPoint(const Pos_XY& pos, bool state = false);

	/******************
	牛B闪闪特效相关接口
	******************/
	//平滑地变化背景色
	void renewBkColor();
	//转换所有图片的颜色
	void renewEffects();

protected:
	//半透明贴图
	void half_transparentimage(IMAGE* dstimg, int x, int y, IMAGE* srcimg);
	//根据当前背景色，修改图片素材的颜色
	void changeEffects(IMAGE* dstimg, IMAGE* srcimg);
	//绘制海面（动态效果）
	void drawSea(int x, int y);
	//刷新爆炸点的容器数据（超过显示时间的节点需要删除）
	void renewBoomPoints();

private:
	IMAGE tankPic[CampCount][ArmorCount][DirectionCount][2];//坦克图片，角标从左到右分别为：阵营、装甲等级、方向、履带切换
	IMAGE mapPic[MapFileCount];//地图图片
	IMAGE bulletPic[DirectionCount];//子弹图片
	IMAGE boomPic[boom_pic_count];//爆炸贴图
	IMAGE bossPic[BossStateCount];//BOSS图片

	//经过处理的图片素材
	IMAGE tankPic_effects[CampCount][ArmorCount][DirectionCount][2];
	IMAGE mapPic_effects[MapFileCount];
	IMAGE bulletPic_effects[DirectionCount];
	IMAGE boomPic_effects[boom_pic_count];
	IMAGE bossPic_effects[BossStateCount];

	float bkHSL[3];//保存背景色HSL模型颜色数据
	float fontHSL[3];//文字颜色
	vector<BoomPoint> boom_points;//保存所有的爆炸点数据
};

