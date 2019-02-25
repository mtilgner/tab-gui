
#include "cell.h"
#include "table.h"

namespace tab
{
Cell::~Cell()
{
	if(Table::cell_mouseover_ == this)
	{
		Table::cell_mouseover_ = 0;
	}
	if(Table::cell_pressed_l_ == this)
	{
		Table::cell_pressed_l_ = 0;
	}
	if(Table::cell_pressed_r_ == this)
	{
		Table::cell_pressed_r_ = 0;
	}
	if(Table::cell_text_input_ == this)
	{
		Table::cell_text_input_ = 0;
	}
}

void Cell::init(Table* table, CellType type, unsigned tabnum, unsigned colnum, unsigned rownum)
{
	table_ = table;
	type_ = type;
	tab_ = tabnum;
	col_ = colnum;
	row_ = rownum;
}

SDL_Rect Cell::rect() const
{
	return table_->rect(this);
}

bool Cell::is_highlighted() const
{
	return table_->is_highlighted(*this);
}


} // namespace tab
