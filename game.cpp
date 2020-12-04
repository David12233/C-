#include "stdafx.h"
#include "game.h"

const int dd_left[3] = { 1,-1,0 };
const int dd_up[3][2] = { 2,0,1,-1,1,1 };
const int dd[4][2] = { 0,-1,1,0,0,0,-1,0 };//左下原点上
const int speed = 1;
const int person_r = 3;
const int wall_r = 1;


void change(char *ss, wchar_t *s) {//wchar_t双字节字符类型或宽字节字符
	int i;
	for (i = 0; ss[i]; i++)
		s[i] = ss[i];
	s[i] = '\0';
}

//返回两点之间的距离的平方   返回两点之间的2范数
int dis(int x1, int y1, int x2, int y2) {
	return (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2);
}

// Command 中公共函数的定义 设置方向键的类型以及停止键  do_type=1 awsd =2 方向键  pause_type = 1 P作为停止键 =2 ESC作为停止键 
void Command::get_kb(const int do_type, const int pause_type) {
	hit = false;//是否点击bool量
	kb = 0;

	if (do_type == 1) {
		if (GetAsyncKeyState('A')) kb |= 1;//左
		if (GetAsyncKeyState('D')) kb |= 2;//右
		if (GetAsyncKeyState('W')) kb |= 4;//上
		if (GetAsyncKeyState('S')) kb |= 8;//下
	}
	else {
		if (GetAsyncKeyState(VK_LEFT)) kb |= 1;
		if (GetAsyncKeyState(VK_RIGHT)) kb |= 2;
		if (GetAsyncKeyState(VK_UP)) kb |= 4;
		if (GetAsyncKeyState(VK_DOWN)) kb |= 8;
	}

	if (pause_type == 1 && GetAsyncKeyState('P'))
		kb |= 16;//5  停止
	else if (pause_type == 2 && GetAsyncKeyState(VK_ESCAPE))
		kb |= 16;//6 ESC

	MOUSEMSG mm;//定义一个关于鼠标的变量
	if (MouseHit()) {//判断当前是否有鼠标动作消息
		mm = GetMouseMsg();//返回保存鼠标信息的结构体，返回鼠标的动作存至鼠标变量mm
		if (mm.mkLButton)//判断鼠标左键是否按下
			hit = true;//若按下则给点击量hit赋值
		pos.ff = mm.x, pos.ss = mm.y;//获得鼠标当前的(x,y)坐标值用来判断点击的是哪个按钮
	}
}

// Map 定义Map的构造函数
Map::Map() {
	ok = false;
	//载入迷宫每一单元，可能出现的边的形式
	loadimage(&p0000, L"image\\0000.jpg", 10, 10, true);//导入图像赋给p0000，设置图像的拉伸长度和宽度为10，调整图像的大小以适应图片
	loadimage(&p0001, L"image\\0001.jpg", 10, 10, true);
	loadimage(&p0010, L"image\\0010.jpg", 10, 10, true);
	loadimage(&p0011, L"image\\0011.jpg", 10, 10, true);
	loadimage(&p0100, L"image\\0100.jpg", 10, 10, true);
	loadimage(&p0101, L"image\\0101.jpg", 10, 10, true);
	loadimage(&p0110, L"image\\0110.jpg", 10, 10, true);
	loadimage(&p0111, L"image\\0111.jpg", 10, 10, true);
	loadimage(&p1000, L"image\\1000.jpg", 10, 10, true);
	loadimage(&p1001, L"image\\1001.jpg", 10, 10, true);
	loadimage(&p1010, L"image\\1010.jpg", 10, 10, true);
	loadimage(&p1011, L"image\\1011.jpg", 10, 10, true);
	loadimage(&p1100, L"image\\1100.jpg", 10, 10, true);
	loadimage(&p1101, L"image\\1101.jpg", 10, 10, true);
	loadimage(&p1110, L"image\\1110.jpg", 10, 10, true);
	loadimage(&p1111, L"image\\1111.jpg", 10, 10, true);

	loadimage(&qizi_, L"image\\qizi.jpg", 120, 60, true);//导入终点的小旗子图像
	SetWorkingImage(&qizi_);//在导入的图片上进行绘图意
	for (int i = 0; i < 4; i++) {
		getimage(&qizi[i][0], i * 30, 0, 30, 30);//在当前的绘图设备上获取图像
		getimage(&qizi[i][1], i * 30, 30, 30, 30);
	}

	SetWorkingImage(NULL);//取消qizi_作为绘图空间

	loadimage(&time_, L"image\\time_.jpg");//导入时间的图片
}

Map::~Map() {

}
//Map中的绘制矩形框的函数
void Map::vis_edge() {//绘制一个大的边缘框
	for (int i = 0; i <= width + 1; i++) {//先绘制
		vis[0][i]++;
		vis[2 * height][i]++;
		edge[0][i]++;
		edge[height + 1][i]++;
	}
	for (int i = 1; i <= height; i++) {
		vis[i][0]++;
		vis[i][width]++;
		edge[i][0]++;
		edge[i][width + 1]++;
	}
}

void Map::vis_circle() {//绘制圆形
	int r = width / 2;
	for (int i = 0; i <= r; i++)
		for (int o = 0; o <= r; o++) {
			if (dis((r - i) * 20 + 10, (r - o) * 20 + 10, 0, 0) - r*r * 20 * 20 > 0) {
				for (int p = 0; p < 4; p++) {
					vis[i * 2 - 1 + dd[p][0]][o + dd[p][1]]++;
					vis[i * 2 - 1 + dd[p][0]][2 * r - o + 1 + dd[p][1]]++;
					vis[4 * r - 2 * i + dd[p][0]][o + dd[p][1]]++;
					vis[4 * r - 2 * i + dd[p][0]][2 * r - o + 1 + dd[p][1]]++;
				}

				ans[i][o]++;
			}
		}

	// left
	for (int i = 0; i <= r; i++)
		for (int o = r; o >= 0; o--)
			if (ans[i][o]) {
				edge[i][o]++;
				edge[width - i + 1][o]++;
				edge[width - i + 1][width - o + 1]++;
				edge[i][width - o + 1]++;
				break;
			}

	// up
	for (int o = r; o >= 0; o--)
		for (int i = r; i >= 0; i--)
			if (ans[i][o]) {
				edge[i][o]++;
				edge[width - i + 1][o]++;
				edge[width - i + 1][width - o + 1]++;
				edge[i][width - o + 1]++;
				break;
			}

	memset(ans, 0, sizeof ans);
}

void Map::vis_heart() {//绘制心形
	int r = (width - 1) / 2;
	int cy = height * 10 + 360;

	for (int i = 1; i <= r; i++) {
		int y2 = int((sqrt(r*r - i*i) + pow(i, (double)2 / 3) + r) * 20 + 0.5);
		int y1 = int((-sqrt(r*r - i*i) + pow(i, (double)2 / 3) + r) * 20 + 0.5);

		for (int o = 1; o <= height; o++) {
			int y = (height - o) * 20 + 10;
			if (y<y1 || y>y2) {

				for (int p = 0; p < 4; p++) {
					vis[2 * o - 1 + dd[p][0]][r + i + 1 + dd[p][1]]++;
					vis[2 * o - 1 + dd[p][0]][r - i + 1 + dd[p][1]]++;
					ans[o][r + i + 1]++, ans[o][r - i + 1]++;
				}
			}
		}
	}

	for (int i = 1; i < r / 2; i++)
		for (int p = 0; p < 4; p++) {
			vis[i * 2 - 1 + dd[p][0]][r + 1 + dd[p][1]]++;
			ans[i][r + 1]++;
		}


	memset(ans, 0, sizeof ans);//初始化内存块 
	//memset()函数原型是extern void *memset(void *buffer, int c, int count)
}	//分别对应的是初始化的地址，初始化的初值，初始化的数据个数

void Map::make_new_map() {
	while (1) {
		vector< pair<int, int> >list;//用来存储访问过的单元格地址
		memset(a, 0, sizeof a);
		memset(ans, 0, sizeof ans);
		memset(vis, 0, sizeof vis);
		memset(edge, 0, sizeof edge);
		memset(b, 0, sizeof b);

		switch (type) {
		case 1:
			vis_edge();
			break;
		case 2:
			vis_circle();
			break;
		case 3:
			vis_heart();
		}

		//  生成起点 都是在迷宫地图的左侧 判断随机起点是否可以向上下右移动一步任然有效
		//设置的终点在右下角
		int cmp = 0;
		int randx = rand() % height + 1, randy = rand() % width + 1;//随机生成起点
		for (int p = 0; p < 4; p++) {
			int xx = 2 * randx - 1 + dd[p][0];
			int yy = randy + dd[p][0];
			if (check(xx, yy) && !vis[xx][yy]) {//判断随机产生的起点是否有效
				list.push_back(pa(xx, yy));//在最后加入一个向量元素（x，y）点
				vis[xx][yy]++;
				cmp++;
			}
		}
		//判断随机产生的初始起点的那些方向是走的通的

		if (cmp)//判断是否生成合格的起点 不合格返回继续循环
			a[randx][randy]++;//随机数起点对应的在数组a中的值置为1
		else
			continue;

		// 迷宫生成
		while (!list.empty()) {//起点的三个方向依次取出直到全部取完
			int rx = rand() % list.size();//
			pair<int, int> x = list[rx];//随机取一个起点三方向中的一点
			vis[x.ff][x.ss]++;//对所在的点先置一
			list.erase(list.begin() + rx);//删除所取的起点

			if (x.ff % 2) {
				if (a[x.ff / 2 + 1][x.ss] ^ a[x.ff / 2 + 1][x.ss + 1]) {//按位异或 判断起点和右侧的点是否相同
					b[x.ff][x.ss]++;//不同
					if (a[x.ff / 2 + 1][x.ss]) {//如果左侧为1  向右侧走一个
						// right
						a[x.ff / 2 + 1][x.ss + 1] = 1;
						for (int i = 0; i < 3; i++) {//分别设置右下侧三个点为通路
							int xx = x.ff + dd_left[i], yy = x.ss + 1;
							if (check(xx, yy) && !vis[xx][yy])
								list.push_back(pa(xx, yy)), vis[xx][yy]++;//把走过的点放入list中
						}
					}
					else {
						// left  
						a[x.ff / 2 + 1][x.ss] = 1;//左侧点为0 先置1 向左走
						for (int i = 0; i < 3; i++) {
							int xx = x.ff + dd_left[i], yy = x.ss - 1;//设置上方的点
							if (check(xx, yy) && !vis[xx][yy])
								list.push_back(pa(xx, yy)), vis[xx][yy]++;
						}
					}
				}
			}
			else {
				if (a[x.ff / 2][x.ss] ^ a[x.ff / 2 + 1][x.ss]) {
					b[x.ff][x.ss]++;
					if (a[x.ff / 2][x.ss]) {//起点为1 向下走并设置 
						// down
						a[x.ff / 2 + 1][x.ss] = 1;
						for (int i = 0; i < 4; i++) {
							int xx = x.ff + dd_up[i][0], yy = x.ss + dd_up[i][1];
							if (check(xx, yy) && !vis[xx][yy])
								list.push_back(pa(xx, yy)), vis[xx][yy]++;
						}
					}
					else {
						// up
						a[x.ff / 2][x.ss] = 1;
						for (int i = 0; i < 4; i++) {
							int xx = x.ff - dd_up[i][0], yy = x.ss + dd_up[i][1];
							if (check(xx, yy) && !vis[xx][yy])
								list.push_back(pa(xx, yy)), vis[xx][yy]++;
						}
					}
				}
			}
		}

		// bfs找是否有通路
		queue< pair<int, int> >q;//定义一个队列
		int diss[100][100], pre[100][100], root = 2, find = 0;//
		min_step = 0;

		int rect[2][4] = { 1,1,height,width,height / 4,1,height * 3 / 4,width };
		//int he[][] = {};

		int ra, xx;
		//根据迷宫地图的形状设置查找的起点和终点
		switch (type) {
		case 1:
			ra = rand() % 2;

			switch (ra) {
			case 0:
				startx = rect[0][0], starty = rect[0][1];
				endx = rect[0][2], endy = rect[0][3];
				break;
			case 1:
				startx = rect[1][0], starty = rect[1][1];
				endx = rect[1][2], endy = rect[1][3];
				break;
			}
			break;
		case 2:
			ra = rand() % 4;
			ra = 1;
			switch (ra) {
			case 0:
				xx = int(height*sqrt(2) / 4.0 + 0.5);
				for (int i = 0; i <= height; i++)
					if (edge[xx][i] && !edge[xx][i + 1]) {
						startx = xx, starty = i + 1;
						endx = height - xx + 1, endy = height - i;
						break;
					}
				break;
			case 1:
				xx = int(height*sqrt(2) / 4.0 + 0.5);
				for (int i = height; i >= 0; i--)
					if (!edge[xx][i] && edge[xx][i + 1]) {
						startx = xx, starty = i;
						endx = height - xx + 1, endy = height - i + 1;
						break;
					}
				break;
			case 2:
				xx = int(height*sqrt(2) / 4.0 + 0.5) + height / 2;
				for (int i = 0; i <= height; i++)
					if (edge[xx][i] && !edge[xx][i + 1]) {
						startx = xx, starty = i + 1;
						endx = height - xx + 1, endy = height - i;
						break;
					}
				break;
			case 3:
				xx = int(height*sqrt(2) / 4.0 + 0.5) + height / 2;
				for (int i = height; i >= 0; i--)
					if (!edge[xx][i] && edge[xx][i + 1]) {
						startx = xx, starty = i;
						endx = height - xx + 1, endy = height - i + 1;
						break;
					}
			}
			break;
		case 3:
			int r = (width - 1) / 2;
			startx = r/2, starty = r/2, endx = height-1, endy = r+1;
		}

		memset(diss, -1, sizeof diss);
		memset(pre, 0, sizeof pre);
		diss[startx][starty] = 1, pre[startx][starty] = -1;
		q.push(pa(startx, starty));

		while (!q.empty()) {//直到队列为空结束循环
			int num = q.size();
			while (num--) {
				pair<int, int>x = q.front();
				q.pop();
				for (int i = 0; i < 4; i++) {//按照左下右上的顺序去寻找出路
					int xx = x.ff * 2 - 1 + dd[i][0], yy = x.ss + dd[i][1];
					int xxx = x.ff + dd[i][0], yyy = x.ss + dd[i][1];
					if (i == 2)yyy++;//如果是2 则代表到达原点  要对yyy加一变为右侧的点
					if (check(xx, yy) && b[xx][yy] && diss[xxx][yyy] == -1) {
						diss[xxx][yyy] = root;//判断能否走的同，如果走的通则赋给diss对应坐标的步数
						pre[xxx][yyy] = (x.ff - 1)*width + x.ss;

						if (xxx == endx&&yyy == endy)
							find = 1;

						q.push(pa(xxx, yyy));//更新当前点的位置
					}
				}
				if (find)break;//如果某一个方向找到则跳出不在进行其他方向寻找
			}
			if (find)break;

			root++;//步数加一
		}

		if (find) {
			int xx = endx, yy = endy;
			while (pre[xx][yy] != -1) {
				ans[xx][yy]=diss[xx][yy];
				v.push_back(pa(xx, yy));

				int t = pre[xx][yy];
				xx = t / width + 1, yy = t % width;
			}

			ans[startx][starty]=1;
			v.push_back(pa(startx, starty));

			min_step = root;

			break;
		}
	}
}

//根据type的值输出迷宫地图
void Map::print(int x, int y, int type) {
	switch (type) {
	case 0:
		putimage(x, y, &p0000);
		break;
	case 1:
		putimage(x, y, &p0001);
		break;
	case 2:
		putimage(x, y, &p0010);
		break;
	case 3:
		putimage(x, y, &p0011);
		break;
	case 4:
		putimage(x, y, &p0100);
		break;
	case 5:
		putimage(x, y, &p0101);
		break;
	case 6:
		putimage(x, y, &p0110);
		break;
	case 7:
		putimage(x, y, &p0111);
		break;
	case 8:
		putimage(x, y, &p1000);
		break;
	case 9:
		putimage(x, y, &p1001);
		break;
	case 10:
		putimage(x, y, &p1010);
		break;
	case 11:
		putimage(x, y, &p1011);
		break;
	case 12:
		putimage(x, y, &p1100);
		break;
	case 13:
		putimage(x, y, &p1101);
		break;
	case 14:
		putimage(x, y, &p1110);
		break;
	case 15:
		putimage(x, y, &p1111);
	}
}

// Menu
//导入游戏界面各张图
Menu::Menu() {
	loadimage(&start, L"image\\start.gif");
	loadimage(&start_, L"image\\start_.gif");
	loadimage(&setting, L"image\\setting.gif");
	loadimage(&setting_, L"image\\setting_.gif");
	loadimage(&exit, L"image\\exit.gif");
	loadimage(&exit_, L"image\\exit_.gif");

	loadimage(&setting_background, L"image\\setting_background.png");
	loadimage(&wasd, L"image\\wasd.png");
	loadimage(&pesc, L"image\\pesc.png");
	loadimage(&setting_background_ingame, L"image\\setting_background_ingame.png");

	loadimage(&game_background_1, L"image\\game_background_1.png");
	loadimage(&game_background_2, L"image\\game_background_2.png");
	loadimage(&circle, L"image\\circle.png");
	loadimage(&heart, L"image\\heart.png");
	loadimage(&__2, L"image\\2.png");
	loadimage(&__3, L"image\\3.png");

	loadimage(&back, L"image\\back.png");
	loadimage(&game_start, L"image\\game_start.png");
	loadimage(&rand_, L"image\\rand.png");

	loadimage(&_0, L"image\\_0.png");
	loadimage(&_1, L"image\\_1.png");
	loadimage(&_2, L"image\\_2.png");
	loadimage(&_3, L"image\\_3.png");
	loadimage(&_4, L"image\\_4.png");
	loadimage(&_5, L"image\\_5.png");
	loadimage(&_6, L"image\\_6.png");
	loadimage(&_7, L"image\\_7.png");
	loadimage(&_8, L"image\\_8.png");
	loadimage(&_9, L"image\\_9.png");

	loadimage(&gameover, L"image\\gameover.png");
	loadimage(&return_, L"image\\return.png");
	loadimage(&victory_, L"image\\victory.png");
}

//定义方向键的类型以及停止键的类型
void Menu::get_kb(const int do_type, const int pause_type) {
	command.get_kb(do_type, pause_type);
}

// 游戏胜利显示胜利子函数 返回值 0 在原界面等待  1 返回游戏主界面 2 游戏结束 
int Menu::victory(const int times, const int step, const int min_step) {
	while (1) {
		command.get_kb(1, 1);

		pair<int, int>pos = command.get_pos();
		bool hit = command.get_hit();

		setfillcolor(BLACK);//设置胜利背景填充颜色为黑色 
		solidrectangle(0, 0, WIDTH, HEIGHT);//绘制背景框的宽和高

		putimage(WIDTH / 4, HEIGHT / 4, &victory_);//在WIDTH/4 HEIGHT/4位置上插入胜利的图片

		//判断鼠标的移动范围 ，若是在所设定的范围内 
		if (WIDTH / 4 + 56 <= pos.ff&&pos.ff <= 164 + WIDTH / 4 && 294 + HEIGHT / 4 <= pos.ss&&pos.ss <= 320 + HEIGHT / 4) {
			putimage(WIDTH / 4 + 49, HEIGHT / 4 + 289, &return_);//在指定位置插入返回游戏（黄色）选项图标
			if (hit)//如果鼠标返回则返回值1  用来判断是返回游戏还是退出游戏
				return 1;// return  
		}

		if (WIDTH / 4 + 492 <= pos.ff&&pos.ff <= 600 + WIDTH / 4 && 295 + HEIGHT / 4 <= pos.ss&&pos.ss <= 321 + HEIGHT / 4) {
			putimage(WIDTH / 4 + 485, HEIGHT / 4 + 290, &gameover);//判断鼠标是不是在结束游戏按钮范围内  根据鼠标动作来操作
			if (hit)//如果未点击鼠标则不动作 只是导入高亮图片 不退出该界面 直至有鼠标点击
				return 2;// gamemover
		}

		int l = times / 20, r = times % 20 * 50;

		// 打印毫秒
		put_number(WIDTH / 4 + 481, HEIGHT / 4 + 82, r % 10);
		if (r / 10)
			put_number(WIDTH / 4 + 469, HEIGHT / 4 + 82, r / 10 % 10);
		if (r / 100)
			put_number(WIDTH / 4 + 457, HEIGHT / 4 + 82, r / 100);

		// 打印秒
		put_number(WIDTH / 4 + 387, HEIGHT / 4 + 82, l % 10);
		if (l / 10)
			put_number(WIDTH / 4 + 375, HEIGHT / 4 + 82, l / 10 % 10);
		if (l / 100)
			put_number(WIDTH / 4 + 363, HEIGHT / 4 + 82, l / 100);

		// 打印步数
		put_number(WIDTH / 4 + 451, HEIGHT / 4 + 144, step % 10);
		if (step / 10)
			put_number(WIDTH / 4 + 439, HEIGHT / 4 + 144, step / 10 % 10);
		if (step / 100)
			put_number(WIDTH / 4 + 427, HEIGHT / 4 + 144, step / 100);

		// 打印最小步数
		put_number(WIDTH / 4 + 451, HEIGHT / 4 + 206, min_step % 10);
		if (min_step / 10)
			put_number(WIDTH / 4 + 439, HEIGHT / 4 + 206, min_step / 10 % 10);
		if (min_step / 100)
			put_number(WIDTH / 4 + 427, HEIGHT / 4 + 206, min_step / 100);

		FlushBatchDraw();//执行未完成的绘制任务
	}
	return 0;//始终在while(1)循环里 不会返回0
}

//在指定位置输出数字0-9的图片
void Menu::put_number(int x, int y, int num) {//在指定位置输出数字
	switch (num) {
	case 0:
		putimage(x, y, &_0);
		break;
	case 1:
		putimage(x, y, &_1);
		break;
	case 2:
		putimage(x, y, &_2);
		break;
	case 3:
		putimage(x, y, &_3);
		break;
	case 4:
		putimage(x, y, &_4);
		break;
	case 5:
		putimage(x, y, &_5);
		break;
	case 6:
		putimage(x, y, &_6);
		break;
	case 7:
		putimage(x, y, &_7);
		break;
	case 8:
		putimage(x, y, &_8);
		break;
	case 9:
		putimage(x, y, &_9);
	}
}

//进入游戏界面
int Menu::print_menu0() {
	setfillcolor(BLACK);//设置填充背景颜色为黑色
	solidrectangle(0, 0, WIDTH, HEIGHT);//画显示窗口大小

	setbkcolor(BLACK);//设置当前绘图背景色
	settextcolor(WHITE);//设置字体颜色

	settextstyle(80, 0, _T("华文行楷"));//设置字体大小高度为80宽度（0）自适应 以及字体样式
	outtextxy(300, 100, _T("随机迷宫小游戏"));//输出文字的内容 以及位置

	pair<int, int>pos = command.get_pos();//获得鼠标的点击位置 以判断进入的下一个界面



	const int left = 460;
	const int up = 300;
	const int high = 50;// 161*37

	//按下游戏开始 返回值1
	if (left <= pos.ff&&pos.ff <= 161 + left&&up <= pos.ss&&pos.ss <= 37 + up) {
		putimage(left, up, &start_);//开始按钮的位置 黄色图标
		if (command.get_hit())
			return 1;
	}
	else
		putimage(left, up, &start);//返回紫色图标 
	
	//如果按下设置键则返回2 进入设置界面
	if (left <= pos.ff&&pos.ff <= 161 + left&&up + high + 37 <= pos.ss&&pos.ss <= 2 * 37 + high + up) {
		putimage(left, up + high + 37, &setting_);//游戏设置的按钮范围  判断是否备案下
		if (command.get_hit())
			return 2;
	}
	else
		putimage(left, up + high + 37, &setting);

	//如果按下游戏退出界面则返回3
	if (left <= pos.ff&&pos.ff <= 161 + left&&up + 2 * high + 2 * 37 <= pos.ss&&pos.ss <= 3 * 37 + 2 * high + up) {
		putimage(left, up + 2 * high + 2 * 37, &exit_);
		if (command.get_hit())
			return 3;
	}
	else
		putimage(left, up + 2 * high + 2 * 37, &exit);

	return 0;//都未按下则返回0  在原来界面等待
}


//开始游戏后的设置界面 包括迷宫的类型 游戏的方式 游戏难度 迷宫的尺寸
//返回值 0原地等待按键按下 1 进入游戏设置界面  2 返回游戏主菜单 3 开始游戏
int Menu::print_menu1(int&maze_type, int&gamemode, int&hard, int&height, int&width) {
	pair<int, int>pos = command.get_pos();
	bool hit = command.get_hit();

	const int aw[4][2] = { 0,0,20,50,20,50,21,49 };//////
	const int ah[4][2] = { 0,0,20,50,20,50,22,50 };//////

	if (gamemode == 1) {//判断选择的游戏模式  如果是游戏模式导入图1 如果是换上模式导入图2
		putimage(0, 0, &game_background_1);
		//进一步根据选择的游戏的难度 导入对应的图像  
		if (hard == 2)
			putimage(45, 445, &__2);
		else if (hard == 3)
			putimage(45, 445, &__3);
	}
	else
		putimage(0, 0, &game_background_2);//观赏模式导入的图像

	if (maze_type == 2)//判断选择的形状为圆
		putimage(43, 168, &circle);
	else if (maze_type == 3)//选择的形状为心型
		putimage(43, 168, &heart);
	
	//根据游戏的形状随机选取游戏的尺寸大小
	if (715 <= pos.ff&&pos.ff <= 904 && 283 <= pos.ss&&pos.ss <= 351) {
		putimage(715, 283, &rand_);
		if (hit) {
			width = rand() % (aw[maze_type][1] - aw[maze_type][0]) + aw[maze_type][0];
			height = rand() % (ah[maze_type][1] - ah[maze_type][0]) + ah[maze_type][0];
			if (maze_type == 2) {
				if (height % 2)
					height--;
				width = height;
			}
			else if (maze_type == 3) {
				if (height % 2)
					height--;
				width = height - 1;
			}
		}
	}// rand

	if (aw[maze_type][0] > width || width > aw[maze_type][1])
		width = aw[maze_type][0];//防止设定的范围超范围
	if (ah[maze_type][0] > height || height > ah[maze_type][1])
		height = ah[maze_type][0];


	//通过点击滑动条来设置迷宫宽度的大小
	if (hit && 133 <= pos.ff&&pos.ff <= 302 && 317 <= pos.ss&&pos.ss <= 327) {
		width = int(((pos.ff - 133) / 169.0)*(aw[maze_type][1] - aw[maze_type][0]) + 0.5) + aw[maze_type][0];
		if (maze_type == 2) {
			if (width % 2)
				width--;
			height = width;
		}
		else if (maze_type == 3) {
			if (width % 2 == 0)
				width--;
			height = width + 2;
		}
			
	}

	if (hit && 444 <= pos.ff&&pos.ff <= 613 && 317 <= pos.ss&&pos.ss <= 327) {
		height = int(((pos.ff - 444) / 169.0)*(ah[maze_type][1] - ah[maze_type][0]) + 0.5) + ah[maze_type][0];
		if (maze_type == 2) {
			if (height % 2)
				height--;
			width = height;
		}
		else if (maze_type == 3) {
			if (height % 2 == 0)
				height--;
			width = height - 2;
		}
	}

	//在指定位置绘制填充圆  对应滑动的元器件
	solidcircle(133 + (width - aw[maze_type][0]) * 169 / (aw[maze_type][1] - aw[maze_type][0]), 322, 6);
	solidcircle(444 + (height - ah[maze_type][0]) * 169 / (ah[maze_type][1] - ah[maze_type][0]), 322, 6);

	//在指定位置输出要显示的数据
	put_number(321, 315, width / 10);
	put_number(334, 315, width % 10);

	put_number(632, 315, height / 10);
	put_number(645, 315, height % 10);

	//选择迷宫的图形为矩形
	if (hit && 127 <= pos.ff&&pos.ff <= 305 && 220 <= pos.ss&&pos.ss <= 248) {
		maze_type = 1;
		width = aw[maze_type][0], height = ah[maze_type][0];
	}

	//选择迷宫的图形为圆
	if (hit && 325 <= pos.ff&&pos.ff <= 503 && 220 <= pos.ss&&pos.ss <= 248){
		maze_type = 2;
		width = aw[maze_type][0], height = ah[maze_type][0];
	}


	//选择迷宫的图形为心形
	if (hit && 523 <= pos.ff&&pos.ff <= 701 && 219 <= pos.ss&&pos.ss <= 247) {
		maze_type = 3;
		width = aw[maze_type][0], height = ah[maze_type][0];
	}


	//游戏模式
	if (hit && 158 <= pos.ff&&pos.ff <= 336 && 397 <= pos.ss&&pos.ss <= 425)
		gamemode = 1;
	
	//官商模式
	if (hit && 326 <= pos.ff&&pos.ff <= 540 && 397 <= pos.ss&&pos.ss <= 425)
		gamemode = 2;

	//游戏模式下选择游戏难度
	if (gamemode == 1) {
		if (hit && 127 <= pos.ff&&pos.ff <= 305 && 494 <= pos.ss&&pos.ss <= 522)
			hard = 1;
		if (hit && 325 <= pos.ff&&pos.ff <= 503 && 494 <= pos.ss&&pos.ss <= 522)
			hard = 2;
		if (hit && 523 <= pos.ff&&pos.ff <= 701 && 493 <= pos.ss&&pos.ss <= 521)
			hard = 3;
	}

	//进入游戏设置界面 返回值1
	if (hit && 35 <= pos.ff&&pos.ff <= 176 && 87 <= pos.ss&&pos.ss <= 126)
		return 1;// choose->setting

	//返回到游戏主菜单 返回值2
	if (635 <= pos.ff&&pos.ff <= 819 && 559 <= pos.ss&&pos.ss <= 612) {
		putimage(635, 559, &back);
		if (hit)
			return 2;// back to menu0
	}
	//开始游戏 返回值3
	if (837 <= pos.ff&&pos.ff <= 1021 && 558 <= pos.ss&&pos.ss <= 611) {
		putimage(837, 558, &game_start);
		if (hit)
			return 3;// game start!!
	}

	return 0;
}

//游戏设定  do_type代表要执行的操作按键  pause_type停止键选择  type 从设置中进入（1）还是游戏中（2）
//
int Menu::print_menu2(int &do_type, int &pause_type, const int type) {


	pair<int, int>pos = command.get_pos();
	bool hit = command.get_hit();

	if (type == 1)//从设置中进入  可以切换到设置
		putimage(0, 0, &setting_background);
	else//从游戏中进入 只能修改操作方式和停止方式
		putimage(0, 0, &setting_background_ingame);

	if (do_type == 2)//设置wasd为操作按键
		putimage(47, 165, &wasd);
	if (pause_type == 2)//设置P为停止键
		putimage(53, 307, &pesc);

	if (hit && 158 <= pos.ff&&pos.ff <= 336 && 220 <= pos.ss&&pos.ss <= 248)
		do_type = 1;//更改操作方式
	if (hit && 362 <= pos.ff&&pos.ff <= 540 && 220 <= pos.ss&&pos.ss <= 248)
		do_type = 2;

	if (hit && 164 <= pos.ff&&pos.ff <= 342 && 362 <= pos.ss&&pos.ss <= 390)
		pause_type = 1;//更改停止按键
	if (hit && 368 <= pos.ff&&pos.ff <= 546 && 362 <= pos.ss&&pos.ss <= 390)
		pause_type = 2;
	
	
	//若是从游戏中进入则返回游戏设置
	if (hit && 905 <= pos.ff&&pos.ff <= 1046 && 90 <= pos.ss&&pos.ss <= 129 && type == 1)
		return 1;	// choose->maze

	//返回至主菜单
	if (847 <= pos.ff&&pos.ff <= 1008 && 566 <= pos.ss&&pos.ss <= 603) {
		putimage(837, 558, &back);
		if (hit)
			return 2;// back to menu0
	}

	return 0;
}


//Game类的构造函数 包括设定初始的游戏方式 游戏难度 操作方式 停止方式 初始大小 迷宫形状 初试时间
Game::Game() {
	gamemode = 1;
	hard = 1;
	do_type = 1;
	pause_type = 1;
	height = width = 0;
	maze_type = 1;
	tt = 0;

	srand((unsigned)time(NULL));
}
  
Game::~Game() {

}

void Game::run_main() {
	int imano = 0;
	pair<int, int>pos;// 像素坐标
	pair<int, int>maze_pos;// 迷宫坐标
	pair<int, int>pp;// 迷宫左上点坐标
	pair<int, int>end;// 终点
	int step = 0, range;//range用来根据难度设定执行的步数

	BeginBatchDraw();//开始执行批量绘图 并且不输出绘图结果 直至遇到FlushBatchDraw或EndBatchDraw

	while (1) {
		switch (imano) {
		case 0:
			while (1) {
				menu.get_kb(do_type, pause_type);

				imano = menu.print_menu0();// 0 原地等待 1开始游戏 2游戏设置 3退出游戏

				FlushBatchDraw();

				if (imano == 3)//退出游戏
					return;
				if (imano)
					break;
			}
			break;
		case 1://点击 开始游戏并设置相应的游戏参数
			while (1) {
				menu.get_kb(do_type, pause_type);

				int num = menu.print_menu1(maze_type, gamemode, hard, height, width);

				if (num == 1)//进入游戏设置 
					imano = 2;
				else if (num == 2)//返回游戏主菜单   对应0原地等待
					imano = 0;
				else if (num == 3) {//开始游戏
					if (gamemode == 1)
						imano = 3;//游戏模式
					else
						imano = 4;//观赏模式
				}

				FlushBatchDraw();

				if (imano != 1)//跳出循环
					break;
			}
			break;
		case 2://游戏设置
			while (1) {
				menu.get_kb(do_type, pause_type);

				int num = menu.print_menu2(do_type, pause_type, 1);

				if (num == 1)
					imano = 1; //返回游戏设定中
				else if (num == 2)//返回游戏主菜单
					imano = 0;

				FlushBatchDraw();

				if (imano != 2)
					break;
			}
			break;
		case 3://游戏模式开始游戏
			step = 0, tt = 0;
			while (1) {
				long t1 = GetTickCount();//获得开始时间

				//迷宫地图是否绘制完成
				if (!map.get_ok()) {//判断迷宫地图是否设置完成
					map.set_height(height);
					map.set_width(width);
					map.set_type(maze_type);

					map.make_new_map();
					map.set_ok(true);

					pos.ff = map.get_starty();
					maze_pos.ff = pos.ff;
					pos.ff = (WIDTH - width * 10) / 2 + (pos.ff - 1) * 10 + 5;

					pos.ss = map.get_startx();
					maze_pos.ss = pos.ss;
					pos.ss = (HEIGHT - height * 10) / 2 + (pos.ss - 1) * 10 + 5;

					pp.ff = (WIDTH - width * 10) / 2;
					pp.ss = (HEIGHT - height * 10) / 2;

					end.ff = map.get_endy(), end.ss = map.get_endx();

					if (hard == 1)//根据难度设置range 的值 对应显示可见地图的大小
						range = 5 + 1;
					else if (hard == 2)
						range = 3 + 1;
					else
						range = 2 + 1;
				}

				menu.get_kb(do_type, pause_type);

				pair<int, int>mouse_pos = menu.get_pos();
				bool hit = menu.get_hit();//鼠标是否点击
				int doo = menu.get_doo();//按键的动作

				int dx = pos.ff - (maze_pos.ff - 1) * 10 - pp.ff;
				int dy = pos.ss - (maze_pos.ss - 1) * 10 - pp.ss;

				int cango = 0;
				//按照左下右上的顺序
				for (int i = 0; i < 4; i++) {
					int xx = 2 * maze_pos.ss - 1 + dd[i][0], yy = maze_pos.ff + dd[i][1];
					if (map.get_b(xx, yy))
						cango += (1 << i);// left down right up
				}

				//左移一步
				if (doo & 1) {// left
					if (dx < person_r + wall_r) {
						if (cango & 1) {
							pos.ff -= speed;
							if (dx < 0) {
								maze_pos.ff--;
								step++;
							}
						}
						else
							pos.ff = pp.ff + (maze_pos.ff - 1) * 10 + person_r + wall_r;
					}
					else
						pos.ff -= speed;
				}
				//右移一步
				if (doo & 2) {// right
					dx += speed;
					if (dx > 10 - person_r - wall_r) {
						if (cango & 4) {
							pos.ff += speed;
							if (dx > 10) {
								maze_pos.ff++;
								step++;
							}
						}
						else
							pos.ff = pp.ff + maze_pos.ff * 10 - person_r - wall_r;
					}
					else
						pos.ff += speed;
				}
				//上移一步
				if (doo & 4) {// up
					dy -= speed;
					if (dy < person_r + wall_r) {
						if (cango & 8) {
							pos.ss -= speed;
							if (dy < 0) {
								maze_pos.ss--;
								step++;
							}
						}
						else
							pos.ss = pp.ss + (maze_pos.ss - 1) * 10 + person_r + wall_r;
					}
					else
						pos.ss -= speed;
				}
				//下移一步
				if (doo & 8) {// down
					dy += speed;
					if (dy > 10 - person_r - wall_r) {
						if (cango & 2) {
							pos.ss += speed;
							if (dy > 10) {
								maze_pos.ss++;
								step++;
							}
						}
						else
							pos.ss = pp.ss + maze_pos.ss * 10 - person_r - wall_r;
					}
					else
						pos.ss += speed;
				}
				//按下停止按键
				if (doo & 16) {
					while (1) {
						menu.get_kb(do_type, pause_type);
						int num = menu.print_menu2(do_type, pause_type, 2);

						FlushBatchDraw();

						if (num == 2)
							break;
					}
					continue;
				}

				setfillcolor(GRAY);
				solidrectangle(0, 0, WIDTH, HEIGHT);

				//到达终点 根据胜利界面根据返回值判断下一步的任务
				if (maze_pos.ff == end.ff&&maze_pos.ss == end.ss) {//判断是否走到终点
					int num = menu.victory(tt, step, map.get_minstep());
					if (num == 1) {//返回游戏开始界面
						imano = 0;
						break;
					}
					else //返回值为2 结束游戏
						return;
				}

				pair<int, int> p1, p2;
				p1 = pa(maze_pos.ff - range, maze_pos.ss - range);
				p2 = pa(maze_pos.ff + range, maze_pos.ss + range);


				for (int i = p1.ff; i <= p2.ff; i++)
					if (i >= 0 && i <= width + 1)
						for (int o = p1.ss; o <= p2.ss; o++)
							if (o >= 0 && o <= height + 1)
								if (map.get_a(o, i)) {
									int type = 0;

									for (int p = 0; p < 4; p++) {
										int xx = 2 * o - 1 + dd[p][0], yy = i + dd[p][1];
										if (map.get_b(xx, yy))
											type += (1 << p);
									}

									map.print(pp.ff + (i - 1) * 10, pp.ss + (o - 1) * 10, type);
								}
								else if (map.get_edge(o, i))//判断是否到达边缘 到达边缘输出 边缘图像
									map.print(pp.ff + (i - 1) * 10, pp.ss + (o - 1) * 10, 0);

				setfillcolor(RED);//绘制小红点 代表当前所在位置
				solidcircle(pos.ff, pos.ss, person_r);

				//将可视范围外的区域全填充为黑色
				setfillcolor(BLACK);
				solidrectangle(0, 0, WIDTH, pos.ss - range * 10 + 1);//绘制无边框的矩形  前两个参数代表矩形的左顶点  后两个是右下顶点坐标
				solidrectangle(0, pos.ss + range * 10 - 1, WIDTH, HEIGHT);
				solidrectangle(0, pos.ss - range * 10, pos.ff - range * 10 + 1, pos.ss + range * 10);
				solidrectangle(pos.ff + range * 10 - 1, pos.ss - range * 10, WIDTH, pos.ss + range * 10);
				
				//绘制可视圆
				draw_circle(pos, range * 10);

				//在终点位置填充终点指示图标
				putimage(pp.ff + (end.ff - 2) * 10, pp.ss + (end.ss - 2) * 10, &map.get_qizi(tt / 4 % 4, 1), SRCAND);
				putimage(pp.ff + (end.ff - 2) * 10, pp.ss + (end.ss - 2) * 10, &map.get_qizi(tt / 4 % 4, 0), SRCINVERT);


				// 秒表
				putimage(WIDTH - 250, 0, &map.get_time_());

				char sss[10];
				wchar_t s[10];
				sprintf_s(sss, sizeof sss, "%3d", tt / 20);
				change(sss, s);

				//设置秒表显示的字体样式及位置
				settextstyle(30, 0, _T("黑体"));
				setbkcolor(WHITE);
				settextcolor(BLACK);
				outtextxy(WIDTH - 140, 45, s);

				setfillcolor(WHITE);
				solidrectangle(850, 160, 1050, 162);


				// step
				draw_girl(doo);//步数行走 有方向键按下才行走
				sprintf_s(sss, sizeof sss, "%3d", step);
				change(sss, s);

				setbkcolor(BLACK);
				settextcolor(WHITE);
				outtextxy(950, 130, s);//输出步数2

				FlushBatchDraw();

				long t2 = GetTickCount();
				if (t2 - t1 < 50)
					Sleep(50 - t2 + t1);

				tt++;
			}
			draw_circle(pos, -1);// clear
			map.set_ok(false);

			break;
		case 4://观赏模式开始游戏
			print_watching();

			imano = 0;
		}
	}
	EndBatchDraw();
}

void Game::draw_circle(pair<int, int>pos, int r) {
	static vector<int>vv;

	if (r == -1) {
		vv.clear();
		return;
	}

	if (vv.size() == 0)
		for (int i = 0; i <= r; i++)
			for (int o = r; o >= 0; o--) {
				if (dis((r - i) * 10 + 5, (r - o) * 10 + 5, 0, 0) - r*r * 10 * 10 >= 0) {
					vv.push_back(o);
					break;
				}
			}

	setfillcolor(BLACK);
	for (int i = 0; i < vv.size(); i++) {
		solidrectangle(pos.ff - r, pos.ss - r + i, pos.ff - r + vv[i], pos.ss - r + i + 1);
		solidrectangle(pos.ff + r - vv[i], pos.ss - r + i, pos.ff + r, pos.ss - r + i + 1);
		solidrectangle(pos.ff - r, pos.ss + r - i - 1, pos.ff - r + vv[i], pos.ss + r - i);
		solidrectangle(pos.ff + r - vv[i], pos.ss + r - i - 1, pos.ff + r, pos.ss + r - i);
	}
}

void Game::draw_girl(const int x) {
	pair<int, int>pos = pa(900, 120);
	static int times = 0;

	setfillcolor(SHALLOW_YELLOW);
	solidcircle(pos.ff, pos.ss - 14, 10);// head

	setlinecolor(SHALLOW_YELLOW);
	for (int i = 0; i < 3; i++)
		line(pos.ff - 1 + i, pos.ss, pos.ff - 1 + i, pos.ss - 6);// neck

	setfillcolor(PINK);
	solidrectangle(pos.ff - 8, pos.ss, pos.ff + 8, pos.ss + 18);

	const int dd_foot[6] = { 1,3,5,7,5,3 };
	const int dd_leg1[6] = { -5,-3,1,3,1,-3 };
	const int dd_leg2[6] = { 24,24,20,20,18,22 };
	int ans = times % 18 / 3;

	for (int i = 0; i < 3; i++) {
		line(pos.ff + i, pos.ss + 20, pos.ff + i - dd_foot[ans], pos.ss + 40);
		line(pos.ff + i, pos.ss + 20, pos.ff + i + dd_foot[ans], pos.ss + 40);
		line(pos.ff + i, pos.ss + 5, pos.ff + i + dd_leg1[ans], pos.ss + dd_leg2[ans]);
	}

	setfillcolor(PINK);
	solidellipse(pos.ff - 13, pos.ss + 18, pos.ff + 13, pos.ss + 28);// shirt

	if (0 < x&&x < 16)
		times++;
}

void Game::print_watching() {
	long t1 = GetTickCount();

	pair<int, int>pp;
	pp.ff = (WIDTH - width * 10) / 2;
	pp.ss = (HEIGHT - height * 10) / 2;

	setfillcolor(BLACK);
	solidrectangle(0, 0, WIDTH, HEIGHT);

	if (!map.get_ok()) {
		map.set_height(height);
		map.set_width(width);
		map.set_type(maze_type);

		map.make_new_map();
		map.set_ok(true);
	}

	bool ok = false;
	int a = 1, b = 1,tt=map.get_sizev()-1;

	while (1) {

		if(!ok)
			for (int i = a; i <= height ; i++) {
				for (int o = b; o <= width ; o++)
					if (map.get_a(i, o)) {
						int type = 0;
						for (int p = 0; p < 4; p++) {
							int xx = 2 * i - 1 + dd[p][0], yy = o + dd[p][1];
							if (map.get_b(xx, yy))
								type += (1 << p);
						}
						map.print(pp.ff + (o - 1) * 10, pp.ss + (i - 1) * 10, type);
						
						b = o + 1;
						ok = true;

						break;
					}

				
				if (ok) {
					a = i;
					ok = false;

					break;
				}

				b = 0;

				if (ok == false && i == height) {
					a = height;
					b = width;
				}
			}

		if (a == height&&b == width&&tt >= 0) {
			setfillcolor(PURPLE);

			int l = pp.ff + (map.get_v(tt).ss - 1) * 10;
			int t = pp.ss + (map.get_v(tt).ff - 1) * 10;
			int r = pp.ff + map.get_v(tt).ss * 10;
			int d = pp.ss + map.get_v(tt).ff * 10;

			solidrectangle(l, t, r, d);

			tt--;
		}

		menu.get_kb(do_type, pause_type);
		pair<int, int>pos = menu.get_pos();
		bool hit = menu.get_hit();


		if (800 <= pos.ff&&pos.ff <= 961 && 660 <= pos.ss&&pos.ss <= 697) {
			putimage(800, 660, &menu.get_exit_());
			if (hit)
				break;
		}
		else
			putimage(800, 660, &menu.get_exit());


		FlushBatchDraw();

		long t2 = GetTickCount();

		//if (t2 - t1 < 100)
			//Sleep(100 - t2 + t1);
		Sleep(40);
	}

	map.set_ok(false);

}