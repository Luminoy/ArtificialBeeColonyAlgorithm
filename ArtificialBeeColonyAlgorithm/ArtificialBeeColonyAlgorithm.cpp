// ArtificialBeeColonyAlgorithm.cpp : �������̨Ӧ�ó������ڵ㡣
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

float domx[2][2] =                          // ������Լ��������
{
	{ -1.0f, 2.0f },{ -1.0f, 2.0f }
};

typedef struct tag_food
{
	float x[X_DIM];                         // ��Ľṹ
	float y;                                // ��Ӧֵ
	int opttimes;                           // ���Ŵ���
} food_t;

const int foodnum = 30;                     /*ʳ������*/
const int employednum = foodnum;            /*��Ӷ������ʳ������, ÿ����Ӷ���������һ��ʳ��*/
const int onlookernum = foodnum;            /*�۲��Ҳ����Ϊʳ������, ������ÿ��ʳ�ﶼ�л������*/
food_t foods[foodnum];                      /*ʳ��, �����н� ��������,����ֵ,��Ӧ��,���Ŵ���*/
const int maxopttimes = 20;                 /*�ֲ��������ֵ*/
const int itermax = 100;                    /*��������*/

int gbest;


// ���� y=4-x1*sin(4*PI*x1)-x2*sin(4*PI*x2+PI+1)

float func_y(float* x)
{
	return 4 - (x[0] * sin(4 * PI * x[0]) - x[1] * sin(4 * PI * x[1] + PI + 1));
}

// ʳ��������ʼ�������������Ÿ���(����)gbest
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

// ����ʳ������³�ʼ��(�൱�ڱ������)

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

// ʳ��ĸ���

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
		// ��Ӷ��(��ʳ����Ϣ��һ���ĸ����������۷����)
		for (i = 0; i < employednum; i++)
		{
			food_update((((int)(RND * 10)) * i) % foodnum);
		}

		// �۲�䣬�����̶�
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

		// ����
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

	fprintf(stdout, "����ֵΪ: %.8f\t��Ϊ: (%.3f, %.3f)\tʱ��: %d\n", \
		foods[gbest].y, foods[gbest].x[0], foods[gbest].x[1], dwT2);

	system("pause");
	return 0;
}
