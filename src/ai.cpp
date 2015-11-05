/* 
* @Author: sxf
* @Date:   2015-05-22 10:12:45
* @Last Modified by:   sxf
* @Last Modified time: 2015-09-16 14:30:38
*/

#include "ai.h"
#include <vector>
#include <algorithm>
#include <unistd.h>

#define MAX_INT 2000000000

class AI_private
{
public:
	AI_private();
	~AI_private();

	inline int  find 			(Chess* chess, bool is_ai, int k);
	inline void find_data 		(Chess* chess, std::vector<char  >& v);
	inline void find_next_chess (Chess* chess, std::vector<Chess*>& c, char user);

	int maxk;
	const static char fx[8][2];
	int ans_x;
	int ans_y;
	bool ans_tx;
};

AI::AI() {
	priv = new AI_private();
}

AI::~AI() {
	delete priv;
}

void AI::RunAI(Chess* chess) {
	priv->find(chess, true, 0);
}

int AI::get_ans_x() {
	return priv->ans_x;
}

int AI::get_ans_y() {
	return priv->ans_y;
}

bool AI::get_ans_tx() {
	return priv->ans_tx;
}

/* private method */

AI_private::AI_private() {
	maxk = 4;
	ans_tx = false; 
}

AI_private::~AI_private() {

}

const char AI_private::fx[8][2] = {
	{-1,-1}, {-1,0}, {-1,1}, {0,1}, {1,1}, {1,0}, {1,-1}, {0,-1}
};

void AI_private::find_data(Chess* chess, std::vector<char>& v) {
	for (int i = 0; i < ChessSize; ++i)
		for (int j = 0; j < ChessSize; ++j) 
			if (chess->getData(i, j) == 0)
				for (int k = 0; k < 8; ++k) {
					int t = chess->getData(i+fx[k][0], j+fx[k][1]);
					if (t != 0 && t != -1) {
						v.push_back(i); v.push_back(j); break;
					}
				}
}

void AI_private::find_next_chess(Chess* chess, std::vector<Chess*>& c, char user) {
	std::vector<char> v; // 找到所有可行位置
	find_data(chess, v);

	for (int i = 0; i < v.size()/2; ++i) {	// 尝试往下面走一步
		Chess* newchess = new Chess(*chess);
		newchess->setOneData(v[i*2], v[i*2+1], user);
		c.push_back(newchess);
	}
}

bool complare_greater(Chess* a, Chess* b)
{
	return a->estimate() > b->estimate();
}

bool complare_lesser(Chess* a, Chess* b)
{
	return a->estimate() < b->estimate();
}

int AI_private::find(Chess* chess, bool is_ai, int k) {
	int e = chess->estimate();
	if (k >= maxk || e <= -(MAX_INT>>1) || e >= (MAX_INT>>1) ) {
		return e;
	}
	char user = is_ai ? 2 : 1;
	std::vector<Chess*> c;
	find_next_chess(chess, c, user);

	// 以后再也不敢轻易相信lambda表达式了
	std::sort(c.begin(), c.end(), [is_ai] (Chess* a, Chess* b) {
		if (is_ai)
			return (a->estimate() > b->estimate()); // 居然是等号在坑人!
		else
			return (a->estimate() < b->estimate());
	});

	//if (is_ai) std::sort(c.begin(), c.end(), complare_greater);
	//else	   std::sort(c.begin(), c.end(), complare_lesser);

	std::vector<int> cc;

	int maxp = k > 4 ? 3 : 7; // 每层最大分析局数
	if (k < 3) maxp = 15;
	int p = 0; int maxormin = is_ai ? -MAX_INT : MAX_INT; Chess* pc = NULL;
	for (auto newchess : c) {
		int realestimated = find(newchess, !is_ai, k+1);
		if (realestimated <= -(MAX_INT>>1)) continue;
		cc.push_back(realestimated);
		if ( (realestimated < maxormin) ^ is_ai ) {
			pc = newchess; 
			maxormin = realestimated;
			if (maxormin >= (MAX_INT>>1)) break;
		}
		++p; if (p == maxp) break;
	}
	if (k == 0 || k ==1) {
		printf("第 %d 层\n", k);
		printf("落子：%d %d\n",chess->getLast_i(), chess->getLast_j());
		for (auto cc : c) {
			printf("%d\t", cc->estimate());
		}
		printf("\n");

		for (auto ccc : cc) {
			printf("%d\t", ccc);
		}
		printf("\n");
		if (is_ai) printf("最大估值 = %d\n", maxormin);
		else 	   printf("最小估值 = %d\n", maxormin);

	}
	if (k == 0) { 
		if ( pc == NULL ) { ans_tx = true; return maxormin; }
		ans_x = pc->getLast_i(); ans_y = pc->getLast_j(); 
		// pc->setEstimate(maxormin);
		printf("落子选择：%d %d\n", ans_x, ans_y);
		if ( maxormin <= -(MAX_INT>>1) ) ans_tx = true;
	}
	for (auto newchess : c) delete newchess;
	return maxormin;
}
