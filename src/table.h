#ifndef TAB_TABLE_H
#define TAB_TABLE_H

#include <list>
#include <vector>

#include "font.h"
#include "scrollbar.h"
#include "cell.h"

namespace tab
{

class Row
{
public:
	Row() = default;
	Row(Table& table, unsigned row, unsigned tabnum = 0);
	Cell& cell(unsigned col);
	Cell& operator[](unsigned col);


private:
	Table* table_ = 0;
	unsigned row_ = 0, tab_ = 0;

};

struct TableAttributes
{
	unsigned
	visible_rows = 1,
				 table_border = 0,
				 cell_border = 0,
				 scrollbar_width = 20
				 ;
	SDL_Rect rect = {0, 0, 0, 0};
	Font* font = 0;
};

class Table
{
public:
	using Renderer = std::function<void (const Table*, SDL_Rect)>;

	Renderer render;
	static std::function<void (SDL_Rect)> default_render_scrollbar;

	Table();
	Table(const TableAttributes& attr);
	Table(const Table& other);
	~Table();

	void show(bool s = true);

	Cell& cell(unsigned rownum, unsigned colnum, unsigned tabnum = 0);
	Cell& table_title();
	Cell& tab_title(unsigned tabnum);
	Cell& column_title(unsigned colnum, unsigned tabnum = 0);

	unsigned add_tab(unsigned n = 1);
	void show_table_title(bool s = true);
	void show_tab_titles(bool s = true);
	void set_attributes(const TableAttributes& a);

	Row add_row(unsigned n = 1, unsigned tabnum = 0);
	void clear_tab(unsigned tabnum);
	void show_column_titles(bool s = true, unsigned tabnum = 0);
	void set_num_columns(unsigned n, unsigned tabnum = 0);
	void set_column_weight(unsigned colnum, unsigned weight, unsigned tabnum = 0);
	void scroll_to_end(unsigned tabnum = 0);
	void set_selected_tab(unsigned tabnum);
	void clear();
	void restore_scroll_pos();
	void restore_selected_tab();


	unsigned num_columns(unsigned tabnum = 0) const;
	unsigned num_rows(unsigned tabnum = 0) const;
	unsigned num_tabs() const;
	SDL_Rect rect(const Cell* c) const;
	const TableAttributes& attributes() const;
	bool visible() const;

	bool is_highlighted(const Cell& c) const;
	unsigned selected_tab() const;

	static void handle(const SDL_Event* e);
	static void logic();
	static void draw();

	static bool point_captured(int screen_x, int screen_y);
	static Cell* get_event_cell();
	static Event get_event();
	static void set_input_cell_focus(Cell* c);
	static void lock_input_cell_focus(bool lock);
	static Font* default_font;

	static Cell* screen_cell(int sx, int sy); //*

private:

	friend class Cell;

	struct Tab
	{
		unsigned
		columns = 1,
				rows = 0,
				has_column_titles = 0,
				has_scrollbar = 0,
				sum_weights = 0,
				scroll_pos = 0
				;

		ScrollBar scrollbar;

		Cell title;
		std::vector<Cell> column_title;
		std::vector<std::vector<Cell>> cell;
		std::vector<unsigned> column_weights;
	};

	struct Callback
	{
		Cell* cell = 0;
		Event event = EVENT_NONE;
		Cell::EventHandler func;
	};

	Cell table_title_;
	std::vector<Tab> tab_;


	unsigned 
	tab_selected_ = 0,
				  has_table_title_ = 0,
				  has_tab_titles_ = 0
				  ;

	unsigned remember_tab_ = 0;
	std::vector<unsigned> remember_scroll_;

	TableAttributes attr_;

	bool show_ = false;

	Font* font_ = 0;

	static Cell
	*cell_mouseover_,
	*cell_pressed_l_,
	*cell_pressed_r_,
	*cell_text_input_
	;
	static bool blink_;
	static bool input_locked_;
	static std::list<Table*> all_;
	static std::list<Callback> callback_;
	static Callback current_callback_;


	static void handle_input_event(const SDL_Event* e);
	bool handle_table_event(const SDL_Event* e);
	Cell* screen_cell_intern(int sx, int sy);
	Font* get_font();
	Tab& get_tab(unsigned& num);
	const Tab& get_tab(unsigned& num) const;
	void stop_input();

	void draw_table();
	void draw_cell(Cell& c);

	void update_scrollbar(Tab& t);

	void init_table();
	void init_tab(Tab& t, unsigned tabnum);
};


} // namespace tab


#endif
