/* 
* @Author: sxf
* @Date:   2015-05-21 10:31:06
* @Last Modified by:   sxf
* @Last Modified time: 2015-05-27 23:25:19
*/
#ifndef CHESS_H
#define CHESS_H

#define ChessSize 15
typedef char byte;
typedef char chess_matrix[ChessSize][ChessSize];

class Chess_private;
class Chess
{
public:
	Chess();
	~Chess();
	Chess(const Chess&);

	int estimate();
	void setEstimate(int x);
	int judge();
	void setData(chess_matrix data);
	void setOneData(int i, int j, char k);
	byte getData(int i, int j);
	void clearAll();
	int getLast_i();
	int getLast_j();
protected:
	Chess_private* priv;
};

#endif // CHESS_H
