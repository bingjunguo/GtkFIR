/* 
* @Author: sxf
* @Date:   2015-05-22 10:11:19
* @Last Modified by:   sxf
* @Last Modified time: 2015-05-27 16:50:21
*/

#ifndef AI_H
#define AI_H

#include "chess.h"

class AI_private;
class AI {
public: 
	AI();
	~AI();

	void RunAI(Chess* chess);
	int get_ans_x();
	int get_ans_y();
	bool get_ans_tx();

protected:
	AI_private* priv;
};

#endif // AI_H
