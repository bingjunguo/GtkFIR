/* 
* @Author: sxf
* @Date:   2015-05-19 14:51:19
* @Last Modified by:   sxf
* @Last Modified time: 2015-05-27 16:59:56
*/

#include "myarea.h"
#include <cairomm/context.h>
#include <gdkmm/general.h>
#include <glibmm/fileutils.h>
#include <string.h>
#include <stdio.h>

class MyArea_private
{
public:
	MyArea_private(MyArea* publ) :
		r(40, 40, 560, 560) , bkg(0, 0, 800, 640)
	{
		this->publ = publ;
		// load some images
		mp_black      = Gdk::Pixbuf::create_from_file("res/b.png");
		mp_white      = Gdk::Pixbuf::create_from_file("res/w.png");
		mp_background = Gdk::Pixbuf::create_from_file("res/bkg.jpg");

		x = r.get_x();
		y = r.get_y();
		height = width  = r.get_width()  / (double)(ChessSize-1);
		ImgSize = width * 0.75;

		is_draw_user_select = false;
	}
	~MyArea_private() {	}

	inline void draw_chess_map(const Cairo::RefPtr<Cairo::Context>& cr);

	inline void draw_background(const Cairo::RefPtr<Cairo::Context>& cr);

	inline void draw_chess(const Cairo::RefPtr<Cairo::Context>& cr);

	inline void draw_select(const Cairo::RefPtr<Cairo::Context>& cr);

	inline void draw_select_one_corner(const Cairo::RefPtr<Cairo::Context>& cr,	
		double x, double y, double xp, double yp);

	inline void draw_chess_one(const Cairo::RefPtr<Cairo::Context>& cr, 
		int i, int j, double imgsize_b, double img_w, Glib::RefPtr<Gdk::Pixbuf> pixbuf);

	bool on_userclick(GdkEventButton* p);
	bool on_mousemove(GdkEventMotion* p);

	inline int pointer2num(double x, double w, double rx, double rw);
	inline double num2pointer(int x, double w, double rx);

	inline int pointer2num_x(double x) {
		pointer2num(x, width, r.get_x(), r.get_width() + width);
	}
	inline int pointer2num_y(double x) {
		pointer2num(x, height, r.get_y(), r.get_height() + height);
	}
	inline double num2pointer_x(int x) {
		num2pointer(x, width, r.get_x());
	}
	inline double num2pointer_y(int x) {
		num2pointer(x, height, r.get_y());
	}

	MyArea* publ;
	Chess* mp_chess;

	Glib::RefPtr<Gdk::Pixbuf> mp_black;
	Glib::RefPtr<Gdk::Pixbuf> mp_white;
	Glib::RefPtr<Gdk::Pixbuf> mp_background;

	int x;
	int y;
	double width;
	double height;
	double ImgSize;
	Gdk::Rectangle r;
	Gdk::Rectangle bkg;

	Gdk::Rectangle user_select;
	bool is_draw_user_select;

	bool can_userclick;
	t_userclick_hook m_signal_userclick;
};


MyArea::MyArea(Chess* chess) {
	priv = new MyArea_private(this);
	priv->mp_chess = chess;
	set_size_request(800,640);
	set_events( Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK );
	signal_button_release_event().connect(
		sigc::mem_fun(*priv, &MyArea_private::on_userclick));
	signal_motion_notify_event ().connect(
		sigc::mem_fun(*priv, &MyArea_private::on_mousemove));
}

MyArea::~MyArea() {
	delete priv;
}

bool MyArea_private::on_userclick(GdkEventButton* p) {
	if (!can_userclick) return false;
	int x = pointer2num_x( p->x );
	int y = pointer2num_y( p->y );
	if (x == -1 || y == -1) return false;

	m_signal_userclick.emit(x, y);
	publ->queue_draw();
	return false;
}

bool MyArea_private::on_mousemove(GdkEventMotion* p) {
	int x = pointer2num_x( p->x );
	int y = pointer2num_y( p->y );
	if (!can_userclick || x == -1 || y == -1) {
		is_draw_user_select = false;
		publ->queue_draw();
		return false;
	}
	
	user_select.set_x( (int)num2pointer_x(x) );
	user_select.set_y( (int)num2pointer_y(y) );
	user_select.set_width( (int)width );
	user_select.set_height( (int)height );

	is_draw_user_select = true;
	publ->queue_draw();
	return false;
}

void MyArea::setCanUserclick(bool b) {
	priv->can_userclick = b;
}

t_userclick_hook MyArea::on_user_click() {
	return priv->m_signal_userclick;
}

bool MyArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	priv->draw_background(cr);
	priv->draw_chess_map(cr);
	priv->draw_chess(cr);
	if (priv->is_draw_user_select) priv->draw_select(cr);
	return true;
}

void MyArea_private::draw_chess_map(const Cairo::RefPtr<Cairo::Context>& cr) 
{
	cr->set_source_rgb(0,0,0);
	cr->set_line_width(1.5);

	for (int i = 0; i < ChessSize; ++i) {
		cr->move_to((double)x + i*width , (double)y );
		cr->line_to((double)x + i*width , (double)y + r.get_height());
		cr->move_to((double)x, (double)y + i*height );
		cr->line_to((double)x + r.get_width(), (double)y + i*height );
	}

	cr->stroke();
}

void MyArea_private::draw_chess_one(const Cairo::RefPtr<Cairo::Context>& cr, 
	int i, int j, double imgsize_b, double img_w, Glib::RefPtr<Gdk::Pixbuf> pixbuf)
{
	cr->translate (x + i * width - imgsize_b,
					y + j * height- imgsize_b);
	cr->scale(img_w, img_w);
	Gdk::Cairo::set_source_pixbuf(cr, pixbuf);
	cr->set_identity_matrix();
	cr->paint();
}

void MyArea_private::draw_chess(const Cairo::RefPtr<Cairo::Context>& cr) 
{
	double imgsize_b = ImgSize * 0.5;
	double img_w = ImgSize / mp_black->get_width(); // 这里必须保障白棋子和黑棋子大小一致

	for (int i = 0; i < ChessSize; ++i)
		for (int j = 0; j < ChessSize; ++j) 
			if (mp_chess->getData(i,j) == 1) {
				draw_chess_one(cr, i, j, imgsize_b, img_w, mp_black);
			} else if (mp_chess->getData(i,j) == 2) {
				draw_chess_one(cr, i, j, imgsize_b, img_w, mp_white);
			}
	cr->stroke();
}

void MyArea_private::draw_background(const Cairo::RefPtr<Cairo::Context>& cr) 
{
	cr->scale(1.5,1.5);
	Gdk::Cairo::set_source_pixbuf(cr, mp_background);
	cr->set_identity_matrix();
	cr->rectangle(bkg.get_x(), bkg.get_y(), bkg.get_width(), bkg.get_height());
	cr->fill();
}

void MyArea_private::draw_select_one_corner(
	const Cairo::RefPtr<Cairo::Context>& cr,
	double x, double y, double xp, double yp)
{
	cr->move_to(x, y);
	cr->line_to(x + xp, y);
	cr->move_to(x, y);
	cr->line_to(x, y + yp);
}

void MyArea_private::draw_select(const Cairo::RefPtr<Cairo::Context>& cr) {
	cr->set_source_rgb(0,1,0);
	cr->set_line_width(1.0);
	
	draw_select_one_corner(cr, user_select.get_x(), user_select.get_y(), 10, 10);
	draw_select_one_corner(cr, user_select.get_x() + user_select.get_width() , user_select.get_y(), -10, 10);
	draw_select_one_corner(cr, user_select.get_x(), user_select.get_y() + user_select.get_height(), 10, -10);
	draw_select_one_corner(cr, user_select.get_x() + user_select.get_width(),
							   user_select.get_y() + user_select.get_height(), -10, -10);

	cr->stroke();
}

int MyArea_private::pointer2num(double x, double w, double rx, double rw) { 
	double half_block = w * 0.5;
	double p = x + half_block - rx;
	if ( 0 < p && p < rw) return (int)(p / w);
	return -1;
}

double MyArea_private::num2pointer(int x, double w, double rx) { 
	double half_block = w * 0.5;
	return x * w - half_block + rx;
}