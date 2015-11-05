/* 
* @Author: sxf
* @Date:   2015-05-19 11:22:49
* @Last Modified by:   sxf
* @Last Modified time: 2015-09-16 14:31:16
*/

#include "game.h"
#include <stdio.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <gtkmm.h>
#include <glibmm/threads.h>
#include <string.h>
#include "myarea.h"
#include "chess.h"
#include "ai.h"

class Game_private
{
public:
	Game_private(Game* game)
	{
		this->game = game; 
		mp_chess = new Chess();
		mp_area = new MyArea(mp_chess);
		mp_AI = new AI();
		srand(time(NULL));
	}
	~Game_private() {
		delete mp_chess;
		delete mp_area;
	}
	
	void gameBegin();
	void gameOver();
	void gameWin();
	void usergo(int x, int y);
	void runAI();

 	Game* game;
 	Chess* mp_chess;
	MyArea* mp_area;
	AI* mp_AI;
};

Game::Game()
{
	priv = new Game_private(this);
	this->add(*(priv->mp_area));
	set_events( Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK );
	priv->mp_area->show();
	priv->mp_area->on_user_click().connect(
		sigc::mem_fun(*priv, &Game_private::usergo));
	priv->gameBegin();
}

Game::~Game() {
	delete priv;
}

void Game_private::gameBegin() {
	mp_chess->clearAll();
	mp_area->setCanUserclick(true);
}

void Game_private::gameOver() {
	printf("YOU LOSE\n");
}

void Game_private::gameWin() {
	printf("YOU WIN\n");
}

void Game_private::usergo(int x, int y) {
	if (mp_chess->getData(x, y) != 0) return;
	mp_chess->setOneData(x, y, 1);
	mp_area->setCanUserclick(false);
	int judge_ans = mp_chess->judge();
	if (judge_ans == 1) { gameWin(); return; }
	if (judge_ans == 2) { gameOver(); return; }
	Glib::Threads::Thread::create( sigc::mem_fun(*this, &Game_private::runAI) );
}


void Game_private::runAI() {
	printf("Thread AI run\n");
	mp_AI->RunAI(mp_chess);
	if (mp_AI->get_ans_tx()) { printf("电脑投降了\n"); gameWin(); return;  }
	mp_chess->setOneData(mp_AI->get_ans_x(), mp_AI->get_ans_y(), 2);
	int judge_ans = mp_chess->judge();
	if (judge_ans == 1) { gameWin(); return; }
	if (judge_ans == 2) { gameOver(); return; }
	mp_area->queue_draw();
	mp_area->setCanUserclick(true);
}