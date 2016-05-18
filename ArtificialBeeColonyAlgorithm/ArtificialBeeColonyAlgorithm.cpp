// ArtificialBeeColonyAlgorithm.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <windows.h>

#define PI         3.1415926f
#define X_DIM      2
#define RND        ((float)rand() / (RAND_MAX + 1))

float domx[2][2] =                          // 定义域约束，区间
{
	{ -1.0f, 2.0f },{ -1.0f, 2.0f }
};

typedef struct tag_food
{
	float x[X_DIM];                         // 解的结构
	float y;                                // 适应值
	int opttimes;                           // 最优次数
} food_t;

const int foodnum = 30;                     /*食物数量*/
const int employednum = foodnum;            /*雇佣兵等于食物数量, 每个雇佣兵负责更新一个食物*/
const int onlookernum = foodnum;            /*观察蜂也设置为食物数量, 理论上每个食物都有机会提高*/
food_t foods[foodnum];                      /*食物, 即可行解 还包含了,函数值,适应度,最优次数*/
const int maxopttimes = 20;                 /*局部最优最大值*/
const int itermax = 100;                    /*迭代次数*/

int gbest;


// 函数 y=4-x1*sin(4*PI*x1)-x2*sin(4*PI*x2+PI+1)

float func_y(float* x)
{
	return 4 - (x[0] * sin(4 * PI * x[0]) - x[1] * sin(4 * PI * x[1] + PI + 1));
}

// 食物的随机初始化，并挑出最优个体(索引)gbest
void foods_init()
{
	register int i, j;

	for (i = 0; i < foodnum; i++)
	{
		for (j = 0; j < X_DIM; j++)
		{
			foods[i].x[j] = domx[j][0] + (domx[j][1] - domx[j][0]) * RND;   
		}

		foods[i].y = func_y(foods[i].x);
		foods[i].opttimes = 0;
	}

	gbest = 0;
	for (i = 1; i < foodnum; i++)
	{
		if (foods[i].y > foods[gbest].y)
		{
			gbest = i;
		}
	}
}

// 单个食物的重新初始化(相当于变异操作)

void food_init_s(int idx)
{
	register int i;

	for (i = 0; i < X_DIM; i++)
	{
		foods[idx].x[i] = domx[i][0] + (domx[i][1] - domx[i][0]) * RND;
	}

	foods[idx].y = func_y(foods[idx].x);
	foods[idx].opttimes = 0;

	if (idx != gbest)
	{
		if (foods[idx].y > foods[gbest].y)
		{
			gbest = idx;
		}
	}
	else
	{
		gbest = 0;
		for (i = 1; i < foodnum; i++)
		{
			if (foods[i].y > foods[gbest].y)
			{
				gbest = i;
			}
		}
	}
}

// 食物的更新

void food_update(int idx)
{
	food_t tmpfood;
	int idx1, dimi;

	while ((idx1 = (int)(RND * foodnum)) == idx);

	dimi = (int)(RND * X_DIM);

	memcpy(tmpfood.x, foods[idx].x, sizeof(foods[idx].x));

	tmpfood.x[dimi] += (foods[idx1].x[dimi] - tmpfood.x[dimi]) * (RND - 0.5f) * 2;

	if (tmpfood.x[dimi] < domx[dimi][0])
	{
		tmpfood.x[dimi] = domx[dimi][0];
	}
	else if (tmpfood.x[dimi] > domx[dimi][1])
	{
		tmpfood.x[dimi] = domx[dimi][1];
	}

	tmpfood.y = func_y(tmpfood.x);

	if (tmpfood.y > foods[idx].y)
	{
		memcpy(foods + idx, &tmpfood, sizeof(food_t));
		if (foods[idx].y > foods[gbest].y)
		{
			gbest = idx;
		}
	}
	else
	{
		foods[idx].opttimes++;
	}
}


int main()
{
	DWORD dwT1, dwT2;
	int iter, cnt, optidx;
	register int i;
	float fit;

	srand((unsigned int)time(NULL));


	dwT1 = GetTickCount();

	foods_init();

	
	for (iter = 0; iter < itermax; iter++)
	{	
		// 雇佣蜂(将食物信息以一定的概率与其他蜜蜂分享)
		for (i = 0; i < employednum; i++)
		{
			food_update((((int)(RND * 10)) * i) % foodnum);
		}

		// 观察蜂，非轮盘赌
		cnt = 0;
		while (cnt < onlookernum)
		{
			for (i = 0; i < foodnum; i++)
			{
				fit = (0.6f * (foods[i].y / foods[gbest].y)) + 0.1f;
				if (RND < fit)
				{
					food_update(i);
					if (++cnt == onlookernum)
					{
						break;
					}
				}
			}
		}

		// 侦查蜂
		optidx = 0;
		for (i = 1; i < foodnum; i++)
		{
			if (foods[i].opttimes > foods[optidx].opttimes)
			{
				optidx = i;
			}
		}

		if (foods[optidx].opttimes > maxopttimes)
		{
			food_init_s(optidx);
		}
	}

	dwT2 = GetTickCount() - dwT1;

	fprintf(stdout, "函数值为: %.8f\t解为: (%.3f, %.3f)\t时间: %d\n", \
		foods[gbest].y, foods[gbest].x[0], foods[gbest].x[1], dwT2);

	system("pause");
	return 0;
}
