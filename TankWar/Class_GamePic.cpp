#include <iostream>
#include "Class_GamePic.h"
#include "Class_Timer.h"

const float max_val[3] = { 360.f,0.7f,0.3f };//控制HSL最大取值
const float min_val[3] = { 0.f,0.4f,0.f };//控制HSL最小取值
const float font_to_bk[3] = { 180.f,1 - max_val[1],1 - max_val[2] };//字体颜色和背景色的差距

Class_GamePic::Class_GamePic()
{
	int scrWidth, scrHeight;//屏幕宽高
	RECT rect;//一个矩形
	LOGFONT f;//保存字体设置

	//调整窗口
	initgraph(gamewindow_width, gamewindow_height, NOCLOSE);//初始化绘图界面
	HWND hWnd = GetHWnd();//获取窗口句柄
	SetWindowText(hWnd, _T("廖霞的坦克大战"));

	//设置窗口相对于桌面居中
	//获得屏幕尺寸
	scrWidth = GetSystemMetrics(SM_CXSCREEN);
	scrHeight = GetSystemMetrics(SM_CYSCREEN);
	//取得窗口尺寸
	GetWindowRect(hWnd, &rect);
	//重新设置rect里的值
	rect.left = (scrWidth - (rect.right - rect.left)) / 2;
	rect.top = (scrHeight - (rect.bottom - rect.top)) / 3;
	//移动窗口到中间
	SetWindowPos(hWnd, HWND_TOP, rect.left, rect.top, rect.right, rect.bottom, SWP_NOSIZE);

	//初始化颜色数组
	for (size_t i = 0; i < 3; i++)
	{
		bkHSL[i] = min_val[i];
		fontHSL[i] = bkHSL[i] + font_to_bk[i];
	}

	//加载图片
	IMAGE tmp;//加载大图用的临时对象
	SetWorkingImage(&tmp);//设置工作区域，从tmp中获取相应大小的图片存入其它图片对象
	/******************************************
	注意：本构造函数使用同一个tmp对象加载
	不同尺寸的图片，loadimage中的最后一个
	参数true代表，每次加载图片的时候，会重
	新处理动态存储分配给该对象，以适应图片
	的尺寸。
	******************************************/

	//加载坦克图片
	loadimage(&tmp, _T("PNG"), _T("TANK"), DirectionCount * source_unit_px * 2, CampCount * ArmorCount * source_unit_px, true);
	for (int camp = 0; camp < CampCount; camp++)//控制阵营
	{
		for (int armor = 0; armor < ArmorCount; armor++)//控制装甲等级
		{
			for (int dir = 0; dir < DirectionCount; dir++)//控制方向
			{
				for (int run_state = 0; run_state < 2; run_state++)//控制履带的动态效果
				{
					int srcX = (dir * 2 + run_state) * source_unit_px;// 要获取图像区域左上角 x 坐标
					int srcY = (camp * ArmorCount + armor) * source_unit_px;// 要获取图像区域的左上角 y 坐标
					getimage(&tankPic[camp][armor][dir][run_state], srcX, srcY, source_unit_px, source_unit_px);
					getimage(&tankPic_effects[camp][armor][dir][run_state], srcX, srcY, source_unit_px, source_unit_px);
				}
			}
		}
	}
	cleardevice();//清空对象

	//加载地图图片
	loadimage(&tmp, _T("PNG"), _T("MAP"), MapFileCount * source_map_px, source_map_px, true);
	for (int i = 0; i < MapFileCount; i++)
	{
		int srcX = i * source_map_px;// 要获取图像区域左上角 x 坐标
		int srcY = 0;// 要获取图像区域的左上角 y 坐标
		getimage(mapPic + i, srcX, srcY, source_map_px, source_map_px);
		getimage(mapPic_effects + i, srcX, srcY, source_map_px, source_map_px);
	}
	cleardevice();//清空对象

	//加载子弹图片
	loadimage(&tmp, _T("PNG"), _T("BULLET"), DirectionCount * source_half_map_px, source_half_map_px, true);
	for (int i = 0; i < DirectionCount; i++)
	{
		int srcX = i * source_half_map_px;// 要获取图像区域左上角 x 坐标
		int srcY = 0;// 要获取图像区域的左上角 y 坐标
		getimage(bulletPic + i, srcX, srcY, source_half_map_px, source_half_map_px);
		getimage(bulletPic_effects + i, srcX, srcY, source_half_map_px, source_half_map_px);
	}
	cleardevice();//清空对象

	//加载爆炸图片
	loadimage(&tmp, _T("PNG"), _T("BOOM"), boom_pic_count * source_boom_px, source_boom_px, true);
	for (int i = 0; i < boom_pic_count; i++)
	{
		int srcX = i * source_boom_px;// 要获取图像区域左上角 x 坐标
		int srcY = 0;// 要获取图像区域的左上角 y 坐标
		getimage(boomPic + i, srcX, srcY, source_boom_px, source_boom_px);
		getimage(boomPic_effects + i, srcX, srcY, source_boom_px, source_boom_px);
	}
	cleardevice();//清空对象

	//加载boss图片
	loadimage(&tmp, _T("PNG"), _T("BOSS"), BossStateCount * source_unit_px, source_unit_px, true);
	for (int i = 0; i < BossStateCount; i++)
	{
		int srcX = i * source_unit_px;// 要获取图像区域左上角 x 坐标
		int srcY = 0;// 要获取图像区域的左上角 y 坐标
		getimage(bossPic + i, srcX, srcY, source_unit_px, source_unit_px);
		getimage(bossPic_effects + i, srcX, srcY, source_unit_px, source_unit_px);
	}
	cleardevice();//清空对象

	//加载logo
	//设置字体
	SetWorkingImage(&logoPic);//准备输出文字到该对象
	cleardevice();
	settextcolor(WHITE);
	gettextstyle(&f);                     // 获取当前字体设置
	f.lfQuality = NONANTIALIASED_QUALITY;    // 设置输出效果为抗锯齿  
	f.lfHeight = logo_row / 2 * source_map_px;//字体高度
	//f.lfHeight = source_map_px;//字体高度
	_tcscpy_s(f.lfFaceName, _T("楷体"));    // 设置字体为“楷体”(高版本 VC 推荐使用 _tcscpy_s 函数)
	f.lfWeight = FW_BOLD;				//粗体
	settextstyle(&f);                     // 应用字体样式
	Resize(&logoPic, source_map_px * logo_col, source_map_px * logo_row);//设置logo的宽高
	Resize(&logoPic_effects, source_map_px * logo_col, source_map_px * logo_row);//设置logo的宽高
	rect = { 0,0,logo_col * source_map_px - 1,logo_row * source_map_px - 1 };
	setfillcolor(WHITE);
	drawtext(_T("\n坦克大战"), &rect, DT_CENTER);

	SetWorkingImage(&logoPic_effects);//准备填充砖块
	fill_image(logoPic_effects, mapPic_effects[FileNum_wall]);
	putimage(0, 0, &logoPic, SRCAND);


	//恢复默认工作区为窗口
	SetWorkingImage();
	//设置字体
	settextcolor(HSLtoRGB(fontHSL[0], fontHSL[1], fontHSL[2]));
	gettextstyle(&f);                     // 获取当前字体设置
	f.lfQuality = ANTIALIASED_QUALITY;    // 设置输出效果为抗锯齿  
	f.lfHeight = source_map_px;//字体高度
	_tcscpy_s(f.lfFaceName, _T("楷体"));    // 设置字体为“楷体”(高版本 VC 推荐使用 _tcscpy_s 函数)
	f.lfWeight = FW_BOLD;				//粗体
	settextstyle(&f);                     // 设置字体样式

	//其它设置
	srand(timeGetTime());//设置一个随机种子，主要用于特效切换
	BeginBatchDraw();//开启批量绘图模式
	setbkmode(TRANSPARENT);//透明背景模式（应用于文字输出等）
	setaspectratio(px_multiple, px_multiple);//设置绘图缩放因子（会影响到贴图坐标，所以putimage时以素材大小计算坐标）
}

Class_GamePic::~Class_GamePic()
{
	EndBatchDraw();//结束批量绘图模式
	closegraph();//关闭绘图界面
}

void Class_GamePic::drawTank(const Class_Tank& tank)
{
	Pos_XY px_pos = tank.GetXYPos();
	UnitType type = tank.GetType();
	Armor armor = tank.GetArmorLev();
	Direction dir = tank.GetDirection();
	Camp camp = type == CP ? CampComputerPlayer : CampPlayer;//根据单位类型得出阵营
	half_transparentimage(NULL, (int)px_pos.x, (int)px_pos.y, &tankPic_effects[camp][armor][dir][tank.GetTrackState()]);
}

// 半透明贴图函数
void Class_GamePic::half_transparentimage(IMAGE* dstimg, int x, int y, IMAGE* srcimg)
{
	// 参数：
	//		dstimg：目标 IMAGE（NULL 表示默认窗体）
	//		x, y:	目标贴图位置
	//		srcimg: 源 IMAGE 对象指针

	// 变量初始化
	DWORD* dst = GetImageBuffer(dstimg);
	DWORD* src = GetImageBuffer(srcimg);
	int src_width = srcimg->getwidth();
	int src_height = srcimg->getheight();
	int dst_width = (dstimg == NULL ? getwidth() : dstimg->getwidth());
	int dst_height = (dstimg == NULL ? getheight() : dstimg->getheight());

	// 计算目标贴图区域的实际长宽
	int i_dst_width = (x + src_width * px_multiple > dst_width) ? dst_width - x : src_width * px_multiple;		// 处理超出右边界
	int i_dst_height = (y + src_height * px_multiple > dst_height) ? dst_height - y : src_height * px_multiple;	// 处理超出下边界
	if (x < 0) { src += -x / px_multiple;				i_dst_width -= -x;	x = 0; }				// 处理超出左边界
	if (y < 0) { src += src_width * (-y / px_multiple);	i_dst_height -= -y;	y = 0; }				// 处理超出上边界

	// 修正目标贴图区起始位置
	dst += dst_width * y + x;

	//实现透明贴图（有放大倍数）
	for (int sy = 0; sy < i_dst_height / px_multiple; sy++)
	{
		for (int sx = 0; sx < i_dst_width / px_multiple; sx++)
		{
			//取出源素材数据
			int sa = ((src[sx] & 0xff000000) >> 24);
			int sr = ((src[sx] & 0xff0000) >> 16);	// 源值已经乘过了透明系数
			int sg = ((src[sx] & 0xff00) >> 8);		// 源值已经乘过了透明系数
			int sb = src[sx] & 0xff;				// 源值已经乘过了透明系数
			//处理绘图显存，把每个源的像素，扩充到目标绘图区中，放大数倍
			for (int dy = 0; dy < px_multiple; dy++)//一个像素绘制多行
			{
				for (int dx = 0; dx < px_multiple; dx++)//一个像素绘制多列
				{
					//计算实际绘图的RGB色
					int dr = ((dst[dy * dst_width + dx] & 0xff0000) >> 16);
					int dg = ((dst[dy * dst_width + dx] & 0xff00) >> 8);
					int db = dst[dy * dst_width + dx] & 0xff;
					//应用到目标显存中
					dst[dy * dst_width + dx] = ((sr + dr * (255 - sa) / 255) << 16)
						| ((sg + dg * (255 - sa) / 255) << 8)
						| (sb + db * (255 - sa) / 255);
				}
			}
			dst += px_multiple;//修正目标绘图区到下一个位置
		}
		//控制显存到下一行
		dst += px_multiple * dst_width - i_dst_width;
		src += src_width;
	}
}

void Class_GamePic::drawSea(int x, int y)
{
	static bool flag = false;//决定采用哪张图片，false使用第一张，true使用第二张
	static int num = FileNum_sea1;
	static DWORD timer = Class_Timer::GetGameTime();//计时器
	DWORD now = Class_Timer::GetGameTime();

	//每1秒切换一次图片
	if (now - timer >= 1000)
	{
		timer = now;
		if (flag)
		{
			flag = false;
			num++;
		}
		else
		{
			flag = true;
			num--;
		}
	}
	putimage(x, y, mapPic_effects + num);//绘制对应图形
}

void Class_GamePic::renewBoomPoints()
{
	DWORD now = Class_Timer::GetGameTime();//获取当前时间
	//DWORD now = timeGetTime();//获取当前时间
	for (auto it = boom_points.begin(); it != boom_points.end();)
	{
		if (now - it->add_time > (DWORD)it->duration)
		{
			//如果已经超过爆炸持续时间
			it = boom_points.erase(it);//清除该爆炸数据
		}
		else
		{
			//否则检查下一个节点
			it++;
		}
	}
}

void Class_GamePic::fill_image(IMAGE& dstimg, const IMAGE& srcimg)
{
	//分别获取两张图片的宽高
	int dst_width = dstimg.getwidth();
	int dst_height = dstimg.getheight();
	int src_width = srcimg.getwidth();
	int src_height = srcimg.getheight();
	int index;//控制循环上限
	IMAGE tmp;//保存图片的临时变量

	if (dst_width <= 0 || dst_height <= 0 || src_width <= 0 || src_height <= 0)
	{
		//如果图片任一图片宽高为0，直接退出
		return;
	}

	Resize(&tmp, dst_width, src_width);
	SetWorkingImage(&tmp);//设置当前绘图区准备填充
	index = dst_width % src_width == 0 ? (dst_width / src_width) : (dst_width / src_width + 1);
	for (int i_col = 0; i_col < index; i_col++)
	{
		//先填充一行图像到tmp中
		putimage(i_col * src_width, 0, &srcimg);
	}

	SetWorkingImage(&dstimg);//准备将tmp中的内容填充到dstimg
	index = dst_height % src_height == 0 ? (dst_height / src_height) : (dst_height / src_height + 1);
	for (int i_row = 0; i_row < index; i_row++)
	{
		//把tmp中的内容，看作多行，填充到dstimg
		putimage(0, i_row * src_height, &tmp);
	}
}

void Class_GamePic::drawBullet(Class_Bullet& bullet)
{
	const Pos_XY& pos = bullet.GetXYPos();
	half_transparentimage(NULL, (int)pos.x, (int)pos.y, &bulletPic_effects[bullet.GetDirection()]);
}

void Class_GamePic::drawBooms()
{
	renewBoomPoints();//先刷新爆炸点的数据
	DWORD now = Class_Timer::GetGameTime();//获取当前时间
	//DWORD now = timeGetTime();//获取当前时间
	//把所有的爆炸点贴图显示出来
	for (auto it = boom_points.begin(); it != boom_points.end(); it++)
	{
		int dtime = now - it->add_time;//获取爆炸贴图已经经过的时间
		int pic_index = abs(it->pic_count - abs(it->duration / 2 - dtime) / (it->duration / 2 / it->pic_count) - 1);//绘制第几张图片
		half_transparentimage(NULL, (int)it->pos.x, (int)it->pos.y, boomPic_effects + pic_index);//半透明绘制爆炸贴图
	}
}

void Class_GamePic::drawLogo(bool effect)
{
	if (effect)
	{
		//先处理保存logo的图形对象
		SetWorkingImage(&logoPic_effects);//准备填充砖块
		fill_image(logoPic_effects, mapPic_effects[FileNum_wall]);//填充砖块
		putimage(0, 0, &logoPic, SRCAND);//用字体图片去掉不是字体部分的砖块（用砖块填充字体）
		SetWorkingImage();//恢复默认工作区为窗口
	}
	//绘制LOGO
	putimage((map_col - logo_col) / 2 * source_map_px, 2 * source_map_px, &logoPic, 0x00220326);//逆波兰表示法：DSna
	putimage((map_col - logo_col) / 2 * source_map_px, 2 * source_map_px, &logoPic_effects, SRCPAINT);

	//输出其它信息
	RECT rect;//一个矩形
	rect = { 0,16 * source_map_px,source_gamewindow_width - 1,(16 + 3) * source_map_px - 1 };
	drawtext(_T("按Shift进入坦克！"), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//设置显示内容单行、水平垂直居中
}

void Class_GamePic::addBoomPoint(const Pos_XY& pos, bool state)
{
	BoomPoint tmp;
	tmp.add_time = Class_Timer::GetGameTime();
	//tmp.add_time = timeGetTime();
	tmp.pos = pos;
	if (state)
	{
		//如果是大型爆炸
		tmp.duration = big_boom_time;
		tmp.pic_count = big_boom_count;
	}
	else
	{
		//如果是小型爆炸
		tmp.duration = small_boom_time;
		tmp.pic_count = small_boom_count;
	}
	boom_points.push_back(tmp);//将改数据保存到容器中
}

void Class_GamePic::drawMap(const MapInt(*map)[map_row][map_col])
{
	//根据地图行列绘制基本地形
	for (int row = 0; row < map_row; row++)
	{
		for (int col = 0; col < map_col; col++)
		{
			MapInt temp = (*map)[row][col];
			switch (temp)//根据地形绘图
			{
			case WALL:
				putimage(col * source_map_px, row * source_map_px, mapPic_effects + FileNum_wall);
				break;
			case IRON:
				putimage(col * source_map_px, row * source_map_px, mapPic_effects + FileNum_iron);
				break;
			case BORDER:
				putimage(col * source_map_px, row * source_map_px, mapPic_effects + FileNum_border);
				break;
			case SEA:
				//putimage(col * map_px, row * map_px, mapPic + FileNum_sea0);//暂未添加海面的动态切换效果
				drawSea(col * source_map_px, row * source_map_px);
				break;
			case ICE:
				putimage(col * source_map_px, row * source_map_px, mapPic_effects + FileNum_ice);
				break;
			case HEADQUARTERS_UL:
				half_transparentimage(NULL, col * map_px, row * map_px, bossPic_effects + BossAlive);
				break;
			case HEADQUARTERS_UL_FAIL:
				half_transparentimage(NULL, col * map_px, row * map_px, bossPic_effects + BossDead);
				break;
			default:
				break;
			}
			if (temp<WALL && temp>EMPTY)//绘制残缺的砖块
			{
				putimage(col * source_map_px, row * source_map_px, mapPic_effects + FileNum_wall);//先绘制完整的砖块

				//检查砖块是否残缺（一个完整砖块看作两行两列）
				for (int s_row = 0; s_row < 2; s_row++)
				{
					for (int s_col = 0; s_col < 2; s_col++)
					{
						if ((temp & 1) == 0)//如果末位为0
						{
							//计算要擦除的边界
							int left = col * source_map_px + s_col * source_half_map_px;
							int top = row * source_map_px + s_row * source_half_map_px;
							int right = col * source_map_px + (s_col + 1) * source_half_map_px;
							int bottom = row * source_map_px + (s_row + 1) * source_half_map_px;
							clearrectangle(left, top, right, bottom);//擦除相应区域
						}
						temp = temp >> 1;//右移一位
					}
				}
			}
		}
	}
	//绘制BOSS
	if ((*map)[BossPos.row][BossPos.col]==0xC8)
	{
		half_transparentimage(NULL, BossPos.col * map_px, BossPos.row * map_px, bossPic_effects + BossAlive);
	}
	else
	{
		half_transparentimage(NULL, BossPos.col * map_px, BossPos.row * map_px, bossPic_effects + BossDead);
	}

	//额外信息
	RECT r = { 2 * source_map_px - 1, 0, 20 * source_map_px - 1, 2 * source_map_px - 1 };
	drawtext(_T("ESC=退出 Enter=切换关卡 P=暂停 "), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_EXPANDTABS);
	r = { 2 * source_map_px - 1 ,28 * source_map_px - 1 ,28 * source_map_px - 1 ,30 * source_map_px - 1 };
	drawtext(_T("wasd或上下左右=方向控制 J或Space=开火"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_EXPANDTABS);
	
	//在右上角显示实际玩游戏的时间
	TCHAR game_time_str[32] = { 0 };
	DWORD game_time_int = Class_Timer::GetGameTime() / 1000;//得到经过的秒数
	_stprintf_s(game_time_str, _T("游戏时间：%02ld:%02ld:%02ld"), game_time_int / (60 * 60), game_time_int / 60, game_time_int % 60);
	r = { 2 * source_map_px - 1, 0, 32 * source_map_px - 1, 2 * source_map_px - 1 };
	drawtext(game_time_str, &r, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_EXPANDTABS);
}

void Class_GamePic::drawJungle(const MapInt(*map)[map_row][map_col])
{
	for (int row = 0; row < map_row; row++)
	{
		for (int col = 0; col < map_col; col++)
		{
			if ((*map)[row][col] == JUNGLE)
			{
				//putimage(col * map_px, row * map_px, mapPic + FileNum_jungle);//非透明贴图，草丛有四个角落是黑色
				half_transparentimage(NULL, col * map_px, row * map_px, mapPic_effects + FileNum_jungle);
			}
		}
	}
}

void Class_GamePic::renewBkColor()
{
	const int step = 233;//颜色变化的步长上限
	static bool flag[3] = { 0 };//控制饱和度、亮度是加还是减，0加，1减
	const float multiple[3] = { 222,33333,66666 };//控制步长倍率
	for (size_t i = 0; i < 3; i++)//控制饱和度和亮度
	{
		float increment = rand() % step / multiple[i];//获取一个增量
		float tmp = bkHSL[i] + (float)pow(-1, flag[i]) * increment;
		if (tmp < min_val[i] || tmp > max_val[i])//检查运算结果是否会超过[0,max_val]
		{
			//加、减翻转
			flag[i] = !flag[i];
		}
		bkHSL[i] += (float)pow(-1, flag[i]) * increment;
		//根据背景色设置文字颜色
		fontHSL[i] = bkHSL[i] + font_to_bk[i];
	}
	setbkcolor(HSLtoRGB(bkHSL[0], bkHSL[1], bkHSL[2]));//改变背景色
	settextcolor(HSLtoRGB(fontHSL[0], fontHSL[1], fontHSL[2]));//改变文字颜色
}

void Class_GamePic::renewEffects()
{
	//修改地形素材的颜色
	for (size_t i = 0; i < MapFileCount; i++)
	{
		switch (i)
		{
		case FileNum_iron:
			break;
		case FileNum_border:
			break;
		case FileNum_ice:
			break;
		default:
			changeEffects(mapPic_effects + i, mapPic + i);
			break;
		}
	}
	//修改玩家坦克素材的颜色
	for (int armor = 0; armor < ArmorCount; armor++)//控制装甲等级
	{
		for (size_t dir = 0; dir < DirectionCount; dir++)//控制方向
		{
			for (int run_state = 0; run_state < 2; run_state++)//控制履带的动态效果
			{
				changeEffects(&tankPic_effects[CampPlayer][armor][dir][run_state], &tankPic[CampPlayer][armor][dir][run_state]);
			}
		}
	}
	//修改爆炸图片素材的颜色
	for (size_t i = 0; i < boom_pic_count; i++)
	{
		changeEffects(boomPic_effects + i, boomPic + i);
	}
	//修改boss图片颜色
	for (size_t i = 0; i < BossStateCount; i++)
	{
		changeEffects(bossPic_effects + i, bossPic + i);
	}
}

void Class_GamePic::changeEffects(IMAGE* dstimg, IMAGE* srcimg)
{
	// 变量初始化
	DWORD* dst = GetImageBuffer(dstimg);
	DWORD* src = GetImageBuffer(srcimg);
	int width = srcimg->getwidth();
	int height = srcimg->getheight();

	// 实现颜色转换
	for (int i = 0; i < width * height; i++)
	{
		COLORREF a = src[i] & 0xff000000;//保存源图片的透明度
		float HSL[3] = { 0 };//保存HSL模型颜色数据
		RGBtoHSL(src[i], HSL, HSL + 1, HSL + 2);

		//改变色相
		HSL[0] += bkHSL[0];
		while (HSL[0] >= 360)
		{
			HSL[0] -= 360;
		}

		//应用颜色设置
		COLORREF tmp = HSLtoRGB(HSL[0], HSL[1], HSL[2]);
		tmp = tmp & 0x00ffffff;//清空透明度信息
		//dst[i] = tmp;
		dst[i] = tmp | a;//保留图片素材原本的透明度（不保留原透明度的话，可能影响半透明贴图效果）
	}
}