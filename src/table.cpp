
#include "table.h"

namespace tab
{

std::function<void (SDL_Rect)> Table::default_render_scrollbar = 0;

Font* Table::default_font = 0;
Cell
*Table::cell_mouseover_ = 0,
*Table::cell_pressed_l_ = 0,
*Table::cell_pressed_r_ = 0,
*Table::cell_text_input_ = 0
;

bool Table::blink_ = false;
bool Table::input_locked_ = false;
std::list<Table*> Table::all_;
std::list<Table::Callback> Table::callback_;
Table::Callback Table::current_callback_;

bool in_rect(const SDL_Rect& r, int x, int y)
{
	return (x >= r.x) && (x <= r.x + r.w) && (y >= r.y) && (y <= r.y + r.h);
}

Row::Row(Table& table, unsigned row, unsigned tabnum)
	: table_(&table), row_(row), tab_(tabnum)
{}

Cell& Row::cell(unsigned col)
{
	return table_->cell(row_, col, tab_);
}

Cell& Row::operator[](unsigned col)
{
	return table_->cell(row_, col, tab_);
}

Table::Table()
{
	init_table();
	all_.push_back(this);
}

Table::Table(const TableAttributes& attr)
	: attr_(attr)
{
	init_table();
	all_.push_back(this);
}

Table::Table(const Table& other)
{
	// set table in each cell

	all_.push_back(this);
}

Table::~Table()
{
	stop_input();
	all_.remove(this);
}





void Table::show(bool s)
{
	show_ = s;
}




Cell& Table::cell(unsigned rownum, unsigned colnum, unsigned tabnum)
{
	Tab& t = get_tab(tabnum);
	if(colnum >= t.columns)
	{
		colnum = t.columns - 1;
	}
	if(rownum >= t.rows)
	{
		rownum = t.rows - 1;
	}

	return t.cell[rownum][colnum]; 
}

Cell& Table::table_title()
{
	return table_title_;
}

Cell& Table::tab_title(unsigned tabnum)
{	
	return get_tab(tabnum).title;
}

Cell& Table::column_title(unsigned colnum, unsigned tabnum)
{
	Tab& t = get_tab(tabnum);
	if(colnum >= t.columns)
	{
		colnum = t.columns - 1;
	}

	return t.column_title[colnum];
}






unsigned Table::add_tab(unsigned n)
{
	unsigned s = tab_.size();
	tab_.resize(s + n);
	for(unsigned i = 0; i < n; ++i)
	{
		init_tab(tab_[s + i], s + i);
	}
	return tab_.size();
}

void Table::show_column_titles(bool s, unsigned tabnum)
{
	Tab& t = get_tab(tabnum);
	t.has_column_titles = s ? 1 : 0;

	update_scrollbar(t);
}

void Table::show_table_title(bool s)
{
	has_table_title_ = s ? 1 : 0;

	for(Tab& t : tab_)
	{
		update_scrollbar(t);
	}
}

void Table::show_tab_titles(bool s)
{
	has_tab_titles_ = s ? 1 : 0;

	for(Tab& t : tab_)
	{
		update_scrollbar(t);
	}
}

void Table::set_attributes(const TableAttributes& a)
{
	unsigned r = attr_.visible_rows;
	attr_ = a;
	if(attr_.visible_rows != r)
	{
		for(Tab& t : tab_)
		{
			update_scrollbar(t);
		}
	}
}

Row Table::add_row(unsigned n, unsigned tabnum)
{
	Tab& t = get_tab(tabnum);
	unsigned s = t.rows;
	t.cell.resize(s + n);
	t.rows = s + n;
	for(unsigned i = 0; i < n; ++i)
	{
		std::vector<Cell>& v = t.cell[s + i];
		v.resize(t.columns);
		for(unsigned j = 0; j < t.columns; ++j)
		{
			v[j].init(this, CELL_TABLE, tabnum, j, s + i);
		}
	}

	update_scrollbar(t);

	return Row(*this, s, tabnum);
}

void Table::clear_tab(unsigned tabnum)
{
	Tab& t = get_tab(tabnum);

	// remember
	if(tabnum >= remember_scroll_.size())
	{
		remember_scroll_.resize(tabnum + 1, 0);
	}
	remember_scroll_[tabnum] = t.scroll_pos;
	remember_tab_ = tab_selected_;

	stop_input();
	t.rows = 0;
	t.scroll_pos = 0;
	t.cell.clear();
}

void Table::set_num_columns(unsigned n, unsigned tabnum)
{
	stop_input();
	Tab& t = get_tab(tabnum);
	t.column_title.resize(n);
	t.column_weights.resize(n);
	for(unsigned i = t.columns; i < n; ++i)
	{
		t.column_title[i].init(this, CELL_COLUMN_TITLE, tabnum, i);
		t.column_weights[i] = 1;
		++t.sum_weights;
	}
	for(unsigned i = 0; i < t.cell.size(); ++i)
	{
		auto& v = t.cell[i];
		v.resize(n);
		for(unsigned j = t.columns; j < n; ++j)
		{
			v[j].init(this, CELL_TABLE, tabnum, j, i);
		}
	}
	t.columns = n;
}

void Table::set_column_weight(unsigned colnum, unsigned weight, unsigned tabnum)
{
	Tab& t = get_tab(tabnum);

	if(colnum >= t.columns)
	{
		colnum = t.columns - 1;
	}
	t.sum_weights -= t.column_weights[colnum];
	t.column_weights[colnum] = weight;
	t.sum_weights += weight;
}

void Table::scroll_to_end(unsigned tabnum)
{
	Tab& t = get_tab(tabnum);
	if(t.has_scrollbar)
	{
		unsigned table_rows = attr_.visible_rows - has_table_title_ - has_tab_titles_ - t.has_column_titles;
		unsigned sp = t.rows - table_rows;
		t.scroll_pos = sp;
		//t.scrollbar.set_value(sp);
		update_scrollbar(t);
	}
}

void Table::set_selected_tab(unsigned tabnum)
{
	if(tabnum < tab_.size())
	{
		tab_selected_ = tabnum;
	}
}

void Table::clear()
{
	// remember
	remember_scroll_.resize(tab_.size());
	for(int i = 0; i < remember_scroll_.size(); ++i)
	{
		remember_scroll_[i] = tab_[i].scroll_pos;
	}
	remember_tab_ = tab_selected_;

	stop_input();
	table_title_ = Cell();
	tab_.clear();
	init_table();
	tab_selected_ = 0;
}	

void Table::restore_scroll_pos()
{
	for(int i = 0; i < tab_.size(); ++i)
	{
		Tab& t = tab_[i];
		if(i < remember_scroll_.size())
		{
			t.scroll_pos = remember_scroll_[i];
			update_scrollbar(t);
		}
	}
}

void Table::restore_selected_tab()
{
	if(remember_tab_ < tab_.size())
	{
		tab_selected_ = remember_tab_;
	}
}

unsigned Table::num_columns(unsigned tabnum) const
{
	return get_tab(tabnum).columns;
}

unsigned Table::num_rows(unsigned tabnum) const
{
	return get_tab(tabnum).rows;
}

unsigned Table::num_tabs() const
{
	return tab_.size();
}

SDL_Rect Table::rect(const Cell* c) const
{
	SDL_Rect r = {0, 0, 0, 0};
	const Tab& t = tab_[c->tab_];
	int offs = 0;
	r.h = (attr_.rect.h - 2 * attr_.table_border) / attr_.visible_rows;
	r.x = attr_.rect.x + attr_.table_border;
	r.y = attr_.rect.y + attr_.table_border;
	r.w = attr_.rect.w - 2 * attr_.table_border;

	switch(c->type_)
	{
		case CELL_TABLE_TITLE:

			break;

		case CELL_TAB_TITLE:
			r.y += r.h * has_table_title_;
			r.w /= tab_.size();
			r.x += r.w * c->tab_;
			break;

		case CELL_COLUMN_TITLE:
			r.y += r.h * (has_table_title_ + has_tab_titles_);
			r.w -= t.has_scrollbar * attr_.scrollbar_width;

			for(unsigned i = 0; i < c->col_; ++i)
			{
				offs += t.column_weights[i];
			}
			offs *= r.w;
			offs /= t.sum_weights;
			r.x += offs;

			r.w *= t.column_weights[c->col_];
			r.w /= t.sum_weights;
			break;

		case CELL_TABLE:
			r.y += r.h * (has_table_title_ + has_tab_titles_ + t.has_column_titles + c->row_ - t.scroll_pos);
			r.w -= t.has_scrollbar * attr_.scrollbar_width;

			for(unsigned i = 0; i < c->col_; ++i)
			{
				offs += t.column_weights[i];
			}
			offs *= r.w;
			offs /= t.sum_weights;
			r.x += offs;

			r.w *= t.column_weights[c->col_];
			r.w /= t.sum_weights;
			break;
	}

	return r;
}

const TableAttributes& Table::attributes() const
{
	return attr_;
}

bool Table::visible() const
{
	return show_;
}

bool Table::is_highlighted(const Cell& c) const
{
	// mouse over clickable cell
	if(&c == cell_mouseover_)
	{
		if(c.is_input_cell)
		{
			return true;
		}
		if(c.type_ == CELL_TAB_TITLE)
		{
			return true;
		}
		if(c.event_filter & (EVENT_CLICK_LEFT | EVENT_CLICK_RIGHT) && c.handle)
		{
			return true;
		}
	}
	// selected tab
	if(c.type_ == CELL_TAB_TITLE)
	{
		if(c.tab_ == tab_selected_)
		{
			return true;
		}
	}

	return false;
}

unsigned Table::selected_tab() const
{
	return tab_selected_;
}

void Table::handle(const SDL_Event* e)
{
	int sx, sy;
	SDL_GetMouseState(&sx, &sy);

	// handle scrollbar
	for(auto it = all_.rbegin(); it != all_.rend(); ++it)
	{
		if((*it)->show_)
		{
			Tab& t = (*it)->tab_[(*it)->tab_selected_];
			if(t.has_scrollbar)
			{
				ScrollBar& s = t.scrollbar;
				if(in_rect((*it)->attributes().rect, sx, sy) && e->type == SDL_MOUSEWHEEL)
				{
					s.set_value(s.value() - e->wheel.y);
				}
				else
				{	
					t.scrollbar.handle(e);
				}
				t.scroll_pos = t.scrollbar.value();
			}
		}
	}

	Cell* sc = screen_cell(sx, sy);

	if(sc && sc->handle)
	{
		if(sc != cell_mouseover_)
		{
			if(sc->event_filter & EVENT_MOUSE_OVER)
			{
				Callback cb;
				cb.cell = sc;
				cb.event = EVENT_MOUSE_OVER;
				cb.func = sc->handle;
				callback_.push_back(cb);
			}
		}
	}
	cell_mouseover_ = sc;
	if(cell_pressed_l_ != sc)
	{
		cell_pressed_l_ = 0;
	}
	if(cell_pressed_r_ != sc)
	{
		cell_pressed_r_ = 0;
	}
	switch(e->type)
	{
		case SDL_MOUSEBUTTONDOWN:
			if(e->button.button == SDL_BUTTON_LEFT)
			{
				cell_pressed_l_ = sc;
			}
			else if(e->button.button == SDL_BUTTON_RIGHT)
			{
				cell_pressed_r_ = sc;
			}
			if(!input_locked_)
			{
				cell_text_input_ = 0;
				SDL_StopTextInput();
			}
			break;

		case SDL_MOUSEBUTTONUP:
			if(e->button.button == SDL_BUTTON_LEFT && cell_pressed_l_)
			{
				if(sc->event_filter & EVENT_CLICK_LEFT && sc->handle)
				{
					Callback cb;
					cb.cell = sc;
					cb.event = EVENT_CLICK_LEFT;
					cb.func = sc->handle;
					callback_.push_back(cb);

				}
				if(sc->is_input_cell)
				{
					if(!input_locked_)
					{
						cell_text_input_ = sc;
						SDL_StartTextInput();
					}
				}
				if(sc->type_ == CELL_TAB_TITLE)
				{
					sc->table_->tab_selected_ = sc->tab_;
				}
			}
			else if(e->button.button == SDL_BUTTON_RIGHT && cell_pressed_r_)
			{
				if(sc->event_filter & EVENT_CLICK_RIGHT && sc->handle)
				{
					Callback cb;
					cb.cell = sc;
					cb.event = EVENT_CLICK_RIGHT;
					cb.func = sc->handle;
					callback_.push_back(cb);
				}
			}
			break;

		case SDL_KEYDOWN:
		case SDL_TEXTINPUT:
			if(cell_text_input_)
			{
				handle_input_event(e);
			}
			break;
	}
}

void Table::logic()
{
	for(Callback& cb : callback_)
	{
		current_callback_ = cb;
		cb.func();
	}
	callback_.clear();
}



void Table::draw()
{
	static int blinktimer = 0;
	int time = SDL_GetTicks();
	if(time - blinktimer > 500)
	{
		blink_ = !blink_;
		blinktimer = time;
	}
	for(Table* t : all_)
	{
		if(t->show_)
		{
			t->draw_table();
		}
	}
}

bool Table::point_captured(int screen_x, int screen_y)
{
	for(Table* t : all_)
	{
		if(t->visible() && in_rect(t->attributes().rect, screen_x, screen_y))
		{
			return true;
		}
	}
	return false;
}

Cell* Table::get_event_cell()
{
	return current_callback_.cell;
}

Event Table::get_event()
{
	return current_callback_.event;
}

void Table::set_input_cell_focus(Cell* c)
{
	cell_text_input_ = c;
	SDL_StartTextInput();
}

void Table::lock_input_cell_focus(bool lock)
{
	input_locked_ = lock;
}

Cell* Table::screen_cell(int sx, int sy)
{
	for(auto it = all_.rbegin(); it != all_.rend(); ++it)
	{
		if((*it)->show_)
		{
			if(in_rect((*it)->attr_.rect, sx, sy))
			{
				return (*it)->screen_cell_intern(sx, sy);
			}
		}
	}
	return 0;
}


void Table::handle_input_event(const SDL_Event* e)
{
	std::string& input_string = cell_text_input_->text;
	switch(e->type)
	{
		case SDL_KEYDOWN:
			switch(e->key.keysym.sym)
			{
				case SDLK_BACKSPACE:
					if(!input_string.empty())
					{
						input_string.resize(input_string.size() - 1);
						if(cell_text_input_->event_filter & EVENT_TEXT_CHANGE && cell_text_input_->handle)
						{
							Callback cb;
							cb.cell = cell_text_input_;
							cb.event = EVENT_TEXT_CHANGE;
							cb.func = cell_text_input_->handle;
							callback_.push_back(cb);
						}
					}
					break;

				case SDLK_RETURN:						
					if(cell_text_input_->event_filter & EVENT_TEXT_ENTER && cell_text_input_->handle)
					{
						Callback cb;
						cb.cell = cell_text_input_;
						cb.event = EVENT_TEXT_ENTER;
						cb.func = cell_text_input_->handle;
						callback_.push_back(cb);
					}
					if(!input_locked_)
					{
						SDL_StopTextInput();
						cell_text_input_ = 0;
					}
					break;

				case SDLK_v:
					if(SDL_GetModState() & KMOD_CTRL)
					{
						input_string += SDL_GetClipboardText();
						if(cell_text_input_->event_filter & EVENT_TEXT_CHANGE && cell_text_input_->handle)
						{
							Callback cb;
							cb.cell = cell_text_input_;
							cb.event = EVENT_TEXT_CHANGE;
							cb.func = cell_text_input_->handle;
							callback_.push_back(cb);
						}	
					}
			}		
			break; // SDL_KEYDOWN

		case SDL_TEXTINPUT:

			if(!(SDL_GetModState() & KMOD_CTRL))
			{
				input_string += e->text.text;
				if(cell_text_input_->event_filter & EVENT_TEXT_CHANGE && cell_text_input_->handle)
				{
					Callback cb;
					cb.cell = cell_text_input_;
					cb.event = EVENT_TEXT_CHANGE;
					cb.func = cell_text_input_->handle;
					callback_.push_back(cb);
				}
			}
			break;
	}
}

Cell* Table::screen_cell_intern(int sx, int sy)
{
	unsigned row_height = (attr_.rect.h - 2 * attr_.table_border) / attr_.visible_rows;
	unsigned y = sy - attr_.rect.y - attr_.table_border;
	unsigned rownum = y / row_height;
	Tab& t = tab_[tab_selected_];
	unsigned table_start = has_table_title_ + has_tab_titles_ + t.has_column_titles;
	if(rownum < table_start)
	{
		if(has_table_title_)
		{
			if(in_rect(table_title_.rect(), sx, sy))
			{
				return &table_title_;
			}
		}
		if(has_tab_titles_)
		{
			for(Tab& u : tab_)
			{
				if(in_rect(u.title.rect(), sx, sy))
				{
					return &u.title;
				}
			}
		}
		if(t.has_column_titles)
		{
			for(Cell& c : t.column_title)
			{
				if(in_rect(c.rect(), sx, sy))
				{
					return &c;
				}
			}
		}
	}
	else if(rownum < attr_.visible_rows && rownum - table_start + t.scroll_pos < t.rows)
	{
		auto& v = t.cell[rownum - table_start + t.scroll_pos];
		for(Cell& c : v)
		{
			if(in_rect(c.rect(), sx, sy))
			{
				return &c;
			}
		}
	}

	return 0;
}

Font* Table:: get_font()
{
	return attr_.font ? attr_.font : default_font;
}

Table::Tab& Table::get_tab(unsigned& num)
{
	if(num >= tab_.size())
	{
		num = tab_.size() - 1;
	}
	return tab_[num];
}

void Table::stop_input()
{
	if(cell_text_input_ && cell_text_input_->table_ == this)
	{
		cell_text_input_ = 0;
		SDL_StopTextInput();
		input_locked_ = false;
	}
}

const Table::Tab& Table::get_tab(unsigned& num) const
{
	if(num >= tab_.size())
	{
		num = tab_.size() - 1;
	}
	return tab_[num];
}

void Table::draw_table()
{	
	Tab& t = tab_[tab_selected_];

	if(render)
	{
		unsigned table_rows = attr_.visible_rows - has_table_title_ - has_tab_titles_ - t.has_column_titles;
		unsigned row_height = (attr_.rect.h - 2 * attr_.table_border) / attr_.visible_rows;
		SDL_Rect r;
		r.x = attr_.rect.x + attr_.table_border;
		r.y = attr_.rect.y + row_height * (has_table_title_ + has_tab_titles_ + t.has_column_titles) + attr_.table_border;
		r.w = attr_.rect.w - (2 * attr_.table_border);
		r.h = row_height * table_rows;

		render(this, r);
	}

	if(has_table_title_)
	{
		draw_cell(table_title_);
	}
	if(has_tab_titles_)
	{
		for(Tab& t : tab_)
		{
			draw_cell(t.title);
		}
	}

	if(t.has_column_titles)
	{
		for(Cell& c : t.column_title)
		{
			draw_cell(c);
		}
	}
	if(t.has_scrollbar)
	{
		if(default_render_scrollbar)
		{
			default_render_scrollbar(t.scrollbar.rect());
		}
		else
		{
			t.scrollbar.draw(get_font());
		}
	}
	unsigned table_rows = attr_.visible_rows - has_table_title_ - has_tab_titles_ - t.has_column_titles;
	for(unsigned i = t.scroll_pos; i < t.cell.size() && i < t.scroll_pos + table_rows; ++i)
	{
		auto& v = t.cell[i];
		for(Cell& c : v)
		{
			draw_cell(c);
		}
	}
}


void Table::draw_cell(Cell& c)
{
	SDL_Rect r = rect(&c);
	if(c.render)
	{
		c.render(&c, r);
	}

	TextAttributes& ta = is_highlighted(c) ? c.text_attr_hl : c.text_attr;
	Font* f = ta.font;
	if(!f)
	{
		f = get_font();
	}
	r.x += attr_.cell_border;
	r.w -= 2 * attr_.cell_border;
	r.y += attr_.cell_border;
	r.h -= 2 * attr_.cell_border;

	if(f)
	{
		f->color(ta.r, ta.g, ta.b);
		f->alpha(ta.a);
		std::string text = c.text;
		if(cell_text_input_ == &c)
		{
			text += blink_ ? '_' : ' ';
		}
		print(f, r, c.align, text);
	}
}


void Table::update_scrollbar(Tab& t)
{
	unsigned table_rows = attr_.visible_rows - has_table_title_ - has_tab_titles_ - t.has_column_titles;
	t.has_scrollbar = t.rows > table_rows ? 1 : 0;
	if(t.has_scrollbar)
	{
		unsigned row_height = (attr_.rect.h - 2 * attr_.table_border) / attr_.visible_rows;

		int x = attr_.rect.x + attr_.rect.w - attr_.scrollbar_width - attr_.table_border;
		int y = attr_.rect.y + row_height * (has_table_title_ + has_tab_titles_ + t.has_column_titles) + attr_.table_border;
		int w = attr_.scrollbar_width;
		int h = row_height * table_rows;

		t.scrollbar.position(x, y);
		t.scrollbar.size(w, h);
		t.scrollbar.scale((float) table_rows / (float) t.rows);
		t.scrollbar.range(0, t.rows - table_rows);
		t.scrollbar.smallest(attr_.scrollbar_width);
		t.scrollbar.set_value(t.scroll_pos);
	}

}

void Table::init_table()
{
	tab_.resize(1);
	init_tab(tab_.front(), 0);
	table_title_.init(this, CELL_TABLE_TITLE);
}

void Table::init_tab(Tab& t, unsigned tabnum)
{	
	t.column_title.resize(1);
	t.column_weights.resize(1);
	t.column_title.front().init(this, CELL_COLUMN_TITLE, tabnum);
	t.column_weights.front() = 1;
	t.title.init(this, CELL_TAB_TITLE, tabnum);
	t.sum_weights = 1;
	t.scroll_pos = 0;

	update_scrollbar(t);

}






} // namespace tab
