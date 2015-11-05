/* 
* @Author: sxf
* @Date:   2015-05-19 14:50:36
* @Last Modified by:   sxf
* @Last Modified time: 2015-05-27 13:48:35
*/

#ifndef MYAREA_H
#define MYAREA_H

#include <gtkmm/drawingarea.h>
#include "chess.h"

typedef sigc::signal<void, int, int> t_userclick_hook;

class MyArea_private;
class MyArea : public Gtk::DrawingArea
{
public:
	MyArea(Chess*);
	virtual ~MyArea();

	t_userclick_hook on_user_click();
	void setCanUserclick(bool b);

protected:
	MyArea_private* priv;

	//Override default signal handler:
	virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
};


#endif // MYAREA_H
