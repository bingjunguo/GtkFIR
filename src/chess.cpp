/* 
* @Author: sxf
* @Date:   2015-05-21 10:37:56
* @Last Modified by:   sxf
* @Last Modified time: 2015-09-16 14:42:53
*/

#include "chess.h"
#include <string.h>
#include <stdio.h>
#define SumSize sizeof(byte)*ChessSize*ChessSize
#define MAX_INT 2000000000

class Chess_private
{
public:
	Chess_private() {
		memset(data, 0, SumSize);
		estimate_save = 0;
	}
	~Chess_private() {}
	
	int estimate();
	byte getdata(int i, int j);
	int getsum(int k, int xs = 1);
	int getsumf(int k, int xs = 1);
	byte data[ChessSize][ChessSize];
	int estimate_save;

	int last_i, last_j;
	char last_b;
	
	bool is_have_space;
	static const int fx[4][2];
	static const int qb[4][2];
};

Chess::Chess() {
	priv = new Chess_private();
}

Chess::Chess(const Chess& chess) {
	priv = new Chess_private();
	setData(chess.priv->data);
	priv->estimate_save = chess.priv->estimate_save;
}

Chess::~Chess() {
	delete priv;
}

int Chess::estimate() {
	return priv->estimate_save;
}

void Chess::setEstimate(int x) {
	priv->estimate_save = x;
}

int Chess::getLast_i() {
	return priv->last_i;
}

int Chess::getLast_j() {
	return priv->last_j;
}

void Chess::setData(chess_matrix data) {
	memcpy(priv->data, data, SumSize);
}

void Chess::setOneData(int i, int j, char k) {
	priv->data[i][j] = k;
	priv->last_i = i;
	priv->last_j = j;
	priv->last_b = k;
	int xs = k == 1 ? -1 : 1;
	int t = priv->estimate();
	if ( t == MAX_INT ) priv->estimate_save = xs * t;
	else priv->estimate_save += xs * t;
}

byte Chess::getData(int i, int j) {
	return priv->getdata(i, j);
}

int Chess::judge() {
	for (int i = 0; i < 4; ++i)	{
		int sum = 1;
		sum += priv->getsum(i);
		sum += priv->getsum(i, -1);
		if (sum >= 5) return priv->last_b;
	}
	return 0;
}

void Chess::clearAll() {
	memset(priv->data, 0, SumSize);
}

const int Chess_private::fx[4][2] = {
	{-1,-1}, {-1,0}, {-1,1}, {0,1}
};

byte Chess_private::getdata(int i, int j) {
	if (0 <= i && i < ChessSize  &&  0 <= j && j < ChessSize)
		return data[i][j];
	else
		return -1;
}

int Chess_private::getsum(int k, int xs) {
	int j; int t = -1;
	for (j = 1; 
			(t = getdata(last_i + xs * j * fx[k][0],
						 last_j + xs * j * fx[k][1]) )
				== last_b;
		++j);
	if (t == 0) is_have_space = true;
	else 		is_have_space = false;
	return j-1;
}

int Chess_private::getsumf(int k, int xs) {
	int j; int t = -1;
	for (j = 1; 
			(t = getdata(last_i + xs * j * fx[k][0],
						 last_j + xs * j * fx[k][1]) )
				!= last_b && t != 0 && t != -1;
		++j);
	if (t == 0) is_have_space = true;
	else 		is_have_space = false;
	return j-1;
}

const int Chess_private::qb[4][2] = {
	{1, 5}, {10, 50}, {100, 500}, {1000, 5000}
};

// 不考虑玩家，单独计算最后一个落子的获得的得分
int Chess_private::estimate() {
	int sum = 0;
	for (int i = 0; i < 4; ++i)	{
		int q = 0; int j = 1; int k = 0; int p = 0;
		j += getsum(i);
		if (is_have_space) ++q;
		j += getsum(i, -1);
		if (is_have_space) ++q;

		k += getsumf(i);
		if (is_have_space) ++p;
		k += getsumf(i, -1);
		if (is_have_space) ++p;

		if (j >= 5) return MAX_INT;
		if (q > 0) sum += qb[j-1][q-1];
		if (p > 0) sum += qb[k-1][p-1]*6;
	}
	return sum;
}

