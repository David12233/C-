#ifndef game_h
#define game_h

#include "easyx.h"
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <queue>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <math.h>
#include <set>

using std::vector;//向量类，类似数组，同一类型的数据的组合，可以对向量赋值及初始化，读取各元素的值，在指定位置插入元素而不是像数组那样替代
using std::queue;//类似于栈一样的队列，先入后出原则，有
using std::pair;//是一个结构体类型，用来将两个数据存放为一个数据，两数据的数据类型可以不同，可以通过first和second分别访问两数据

using std::set;//是一个存放同类型的数据集合，该数据集合是经过排序的，且没有重复的元素

#define HEIGHT 720//设置游戏窗口的高度
#define WIDTH 1080//设置游戏窗口的宽度
#define SIZE 10

#define ff first//用于访问pair结构体类型的第一个数据
#define ss second//用于访问pair结构体类型的第二个数据
#define pa(x,y) std::make_pair(x,y)//make_pair()用来产生一个pair数据类型
#define check(x,y) x > 0 && y > 0 && x < 2 * height && y < width

#define GRAY 0x808080 
#define PURPLE 0x9900FF
#define SHALLOW_YELLOW 0xFFF8DC
#define PINK 0xFF1493

/*
1.让迷宫全是墙.
2.选一个单元格作为迷宫的通路，然后把它的邻墙放入列表
3.当列表里还有墙时
1.从列表里随机选一个墙，如果这面墙分隔的两个单元格只有一个单元格被访问过
1.那就从列表里移除这面墙，即把墙打通，让未访问的单元格成为迷宫的通路
2.把这个格子的墙加入列表
2.如果墙两面的单元格都已经被访问过，那就从列表里移除这面墙
*/

class Command {
private:
	int kb;
	pair<int, int>pos;//定义在command类的私有属性下  保护
	bool hit;
public:
	Command() {};
	~Command() {};
	void get_kb(const int,const int);
	pair<int,int> get_pos() {//定义一公共函数以访问command类的私有部分
		return pos;
	}
	bool get_hit() {
		return hit;//访问私有部分的数据类型
	}
	int get_doo() {
		return kb;
	}
};

class Map {
private:
	int type;// 1=rectangle  2=circle  3=heart  设置迷宫的形状
	int height, width;//迷宫的高和宽，最大为49 小于二维数组所能储存的数据
	int a[100][100];// 用于保存迷宫地图的墙和路的信息
	int vis[100][100]; // 绘制迷宫地图时 存储单元格是否被访问过 访问过置1
	int ans[100][100];//用于辅助生成圆形和心型边框
	int b[100][100];//保存到达终点的路径信息
	int edge[100][100];//存放边缘对应的点信息
	int size;// 大小
	bool ok;
	int startx, starty, endx, endy;//开始和结束的位置
	int min_step;//最少的步数
	vector< pair<int, int> >v;//定义关于结构体的向量
	IMAGE p0000, p0001, p0010, p0011, p0100, p0101, p0110, p0111, p1000, p1001, p1010, p1011, p1100, p1101, p1110, p1111;
	IMAGE qizi_,qizi[4][2];
	IMAGE time_;//显示时间的背景图
public:
	Map();//类Map的构造函数
	~Map();//类Map的析构函数
	int get_height() {
		return height;
	}
	int get_width() {
		return width;
	}
	int get_type() {
		return type;
	}
	void set_height(int a) {
		height = a;
	}
	void set_width(int a) {
		width = a;
	}
	void set_type(int a) {
		type = a;
	}
	int get_startx() {
		return startx;
	}
	int get_starty() {
		return starty;
	}
	int get_endx() {
		return endx;
	}
	int get_endy() {
		return endy;
	}
	bool get_ok() {
		return ok;
	}
	void set_ok(bool a) {
		ok = a;
	}
	int get_a(int x, int y) {
		return a[x][y];
	}
	int get_b(int x, int y) {
		return b[x][y];
	}
	int get_edge(int a, int b) {
		return edge[a][b];
	}
	int get_minstep() {
		return min_step;
	}
	int get_ans(int i,int o) {
		return ans[i][o];
	}
	void vis_edge();
	void vis_circle();
	void vis_heart();
	void make_new_map();
	void print(int, int, int);
	IMAGE get_qizi(int x, int y) {
		return qizi[x][y];
	}
	IMAGE get_time_() {
		return time_;
	}
	pair<int,int> get_v(int x) {
		return v[x];
	}
	int get_sizev() {
		return v.size();
	}
};

class Menu {//游戏菜单类
private:
	int width, height, type;
	Command command;
	IMAGE start_, start, setting, setting_, exit, exit_;// 开始界面按钮 分别对应为选中相应按钮时的菜单效果和选中按钮的效果 
	//未选中紫色 选中黄色

	IMAGE setting_background, wasd, pesc, setting_background_ingame;
	IMAGE game_background_1, game_background_2, circle, heart,__2,__3;
	IMAGE back, game_start, rand_;
	IMAGE _1, _2, _3, _4, _5, _6, _7, _8, _9, _0;//0-9对应的数字图片
	IMAGE gameover, return_, victory_;
public:
	Menu();
	int get_height() {
		return height;
	}
	int get_width() {
		return width;
	}
	int get_type() {
		return type;
	}
	pair<int, int> get_pos() {
		return command.get_pos();
	}
	bool get_hit() {
		return command.get_hit();
	}
	//返回按键相应的结果值kb
	int get_doo() {
		return command.get_doo();
	}
	void get_kb(const int,const int);
	int print_menu0(); // 开始界面
	int print_menu1(int&, int &, int&, int&, int&); // 参数设置 游戏设定
	int print_menu2(int&, int&, const int);  // 游戏设置
	//IMAGE get_not(int x);
	void put_number(int, int, int);
	int victory(int,int,int);
	IMAGE get_exit() {
		return exit;
	}
	IMAGE get_exit_() {
		return exit_;
	}
};

class Game {
private:
	Map map;
	Menu menu;
	int maze_type;
	int height, width;
	int gamemode;// 1-普通 2-观赏
	int hard;// 难度 3-2  2-3  1-5
	int do_type;// 1-wasd 2-up...
	int pause_type;// 1-p 2-esc
	int tt;
public:
	Game();
	~Game();
	void set_height() {
		map.set_height(menu.get_height());
	}
	void set_width() {
		map.set_width(menu.get_width());
	}
	void set_type() {
		map.set_type(menu.get_type());
	}
	void run_main();
	void draw_circle(pair<int, int>, int);
	void draw_girl(int);
	void print_watching();
};

#endif