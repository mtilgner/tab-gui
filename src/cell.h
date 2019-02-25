#ifndef TAB_CELL_H
#define TAB_CELL_H

#include <string>
#include <functional>
#include <sstream>

#include "font.h"

namespace tab
{

class Font;
class Table;

enum Event
{
	EVENT_NONE			= 0,
	EVENT_MOVE_IN		= 0x1,	//*
	EVENT_MOVE_OUT		= 0x2,	//*
	EVENT_CLICK_LEFT	= 0x4,
	EVENT_CLICK_RIGHT	= 0x8,
	EVENT_TEXT_CHANGE	= 0x10,
	EVENT_TEXT_ENTER	= 0x20,
	EVENT_MOUSE_OVER	= 0x40
};

enum InputType
{
	INPUT_TEXT,
	INPUT_INT,
	INPUT_FLOAT
};

enum CellType
{
	CELL_TABLE_TITLE,
	CELL_TAB_TITLE,
	CELL_COLUMN_TITLE,
	CELL_TABLE
};

struct TextAttributes
{
	Font* font = 0;
	unsigned char
	r = 255,
	  g = 255,
	  b = 255,
	  a = 255
	  ;
};

struct InputAttributes
{
	InputType type = INPUT_TEXT;
};


class Cell
{
public:
	using Renderer = std::function<void (Cell*, SDL_Rect)>;
	using EventHandler = std::function<void (void)>;

	Cell() = default;
	~Cell();


	std::string text;
	TextAttributes text_attr, text_attr_hl;
	TextAlign align = ALIGN_LEFT;
	bool is_input_cell = false;
	InputAttributes input_attr;
	int event_filter = EVENT_NONE;

	unsigned row() const {return row_;}
	unsigned col() const {return col_;}
	unsigned index() const;
	CellType type() const {return type_;}
	SDL_Rect rect() const;
	bool is_highlighted() const;

	Renderer render;
	EventHandler handle;

	friend class Table;

private:
	Table* table_ = 0;
	unsigned
	row_ = 0,
		 col_ = 0,
		 tab_ = 0
		 ;
	CellType type_ = CELL_TABLE;
	void init(Table* table, CellType type, unsigned tabnum = 0, unsigned colnum = 0, unsigned rownum = 0);
};


template<class T>
Cell& operator<<(Cell& c, T t)
{
	std::ostringstream os;
	os << t;
	c.text += os.str();
	return c;
}

template<class T>
const Cell& operator>>(const Cell& c, T& t)
{
	std::istringstream iss(c.text);
	iss >> t;
	return c;
}




} // namespace tab

#endif
