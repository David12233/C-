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

using std::vector;//�����࣬�������飬ͬһ���͵����ݵ���ϣ����Զ�������ֵ����ʼ������ȡ��Ԫ�ص�ֵ����ָ��λ�ò���Ԫ�ض������������������
using std::queue;//������ջһ���Ķ��У�������ԭ����
using std::pair;//��һ���ṹ�����ͣ��������������ݴ��Ϊһ�����ݣ������ݵ��������Ϳ��Բ�ͬ������ͨ��first��second�ֱ����������

using std::set;//��һ�����ͬ���͵����ݼ��ϣ������ݼ����Ǿ�������ģ���û���ظ���Ԫ��

#define HEIGHT 720//������Ϸ���ڵĸ߶�
#define WIDTH 1080//������Ϸ���ڵĿ��
#define SIZE 10

#define ff first//���ڷ���pair�ṹ�����͵ĵ�һ������
#define ss second//���ڷ���pair�ṹ�����͵ĵڶ�������
#define pa(x,y) std::make_pair(x,y)//make_pair()��������һ��pair��������
#define check(x,y) x > 0 && y > 0 && x < 2 * height && y < width

#define GRAY 0x808080 
#define PURPLE 0x9900FF
#define SHALLOW_YELLOW 0xFFF8DC
#define PINK 0xFF1493

/*
1.���Թ�ȫ��ǽ.
2.ѡһ����Ԫ����Ϊ�Թ���ͨ·��Ȼ���������ǽ�����б�
3.���б��ﻹ��ǽʱ
1.���б������ѡһ��ǽ���������ǽ�ָ���������Ԫ��ֻ��һ����Ԫ�񱻷��ʹ�
1.�Ǿʹ��б����Ƴ�����ǽ������ǽ��ͨ����δ���ʵĵ�Ԫ���Ϊ�Թ���ͨ·
2.��������ӵ�ǽ�����б�
2.���ǽ����ĵ�Ԫ���Ѿ������ʹ����Ǿʹ��б����Ƴ�����ǽ
*/

class Command {
private:
	int kb;
	pair<int, int>pos;//������command���˽��������  ����
	bool hit;
public:
	Command() {};
	~Command() {};
	void get_kb(const int,const int);
	pair<int,int> get_pos() {//����һ���������Է���command���˽�в���
		return pos;
	}
	bool get_hit() {
		return hit;//����˽�в��ֵ���������
	}
	int get_doo() {
		return kb;
	}
};

class Map {
private:
	int type;// 1=rectangle  2=circle  3=heart  �����Թ�����״
	int height, width;//�Թ��ĸߺͿ����Ϊ49 С�ڶ�ά�������ܴ��������
	int a[100][100];// ���ڱ����Թ���ͼ��ǽ��·����Ϣ
	int vis[100][100]; // �����Թ���ͼʱ �洢��Ԫ���Ƿ񱻷��ʹ� ���ʹ���1
	int ans[100][100];//���ڸ�������Բ�κ����ͱ߿�
	int b[100][100];//���浽���յ��·����Ϣ
	int edge[100][100];//��ű�Ե��Ӧ�ĵ���Ϣ
	int size;// ��С
	bool ok;
	int startx, starty, endx, endy;//��ʼ�ͽ�����λ��
	int min_step;//���ٵĲ���
	vector< pair<int, int> >v;//������ڽṹ�������
	IMAGE p0000, p0001, p0010, p0011, p0100, p0101, p0110, p0111, p1000, p1001, p1010, p1011, p1100, p1101, p1110, p1111;
	IMAGE qizi_,qizi[4][2];
	IMAGE time_;//��ʾʱ��ı���ͼ
public:
	Map();//��Map�Ĺ��캯��
	~Map();//��Map����������
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

class Menu {//��Ϸ�˵���
private:
	int width, height, type;
	Command command;
	IMAGE start_, start, setting, setting_, exit, exit_;// ��ʼ���水ť �ֱ��ӦΪѡ����Ӧ��ťʱ�Ĳ˵�Ч����ѡ�а�ť��Ч�� 
	//δѡ����ɫ ѡ�л�ɫ

	IMAGE setting_background, wasd, pesc, setting_background_ingame;
	IMAGE game_background_1, game_background_2, circle, heart,__2,__3;
	IMAGE back, game_start, rand_;
	IMAGE _1, _2, _3, _4, _5, _6, _7, _8, _9, _0;//0-9��Ӧ������ͼƬ
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
	//���ذ�����Ӧ�Ľ��ֵkb
	int get_doo() {
		return command.get_doo();
	}
	void get_kb(const int,const int);
	int print_menu0(); // ��ʼ����
	int print_menu1(int&, int &, int&, int&, int&); // �������� ��Ϸ�趨
	int print_menu2(int&, int&, const int);  // ��Ϸ����
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
	int gamemode;// 1-��ͨ 2-����
	int hard;// �Ѷ� 3-2  2-3  1-5
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