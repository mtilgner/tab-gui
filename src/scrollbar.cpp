	
	#include "scrollbar.h"
	#include "font.h"

	namespace tab
	{
	
	ScrollBar* ScrollBar::active_scrollbar_ = 0;
	int ScrollBar::scroll_rel_drag_offs_ = 0;

	void ScrollBar::size(int w, int h)
	{
		w_ = w;
		h_ = h;
	}

	void ScrollBar::position(int x, int y)
	{
		x_ = x;
		y_ = y;
	}

	void ScrollBar::range(float min, float max)
	{
		min_ = min;
		max_ = max;

		if(val_ < min_) val_ = min_;
		if(val_ > max_) val_ = max_;
	}

	void ScrollBar::scale(float s)
	{
		scale_ = s;
	}

	void ScrollBar::smallest(int s)
	{
		smallest_ = s;
	}

	float ScrollBar::value() const
	{
		return val_;
	}

	void ScrollBar::set_value(float val)
	{
		if(val < min_) val = min_;
		else if(val	> max_) val = max_;

		val_ = val;
	}

	void ScrollBar::handle(const SDL_Event* event)
	{
		if(min_ == max_) return;
		SDL_Rect r = rect();
		if(event->type == SDL_MOUSEBUTTONDOWN)
		{
			int x = event->button.x;
			int y = event->button.y;
			if(
				x >= r.x
				&& x < r.x + r.w
				&& y >= r.y
				&& y < r.y + r.h
			)
			{
				active_scrollbar_ = this;
				scroll_rel_drag_offs_ = y - r.y;
			}
		}
		else if(event->type == SDL_MOUSEBUTTONUP)
		{
			active_scrollbar_ = 0;
		}
		else if(event->type == SDL_MOUSEMOTION && active_scrollbar_ == this)
		{
			int y = event->motion.y - scroll_rel_drag_offs_ - y_;
			float s = step();
			float d = y / s;
			set_value(min_ + d);
		}
	}

	void ScrollBar::draw(Font* font) const
	{
		if(min_ == max_) return;
		font->color(r_, g_, b_);
		font->draw_rect(rect());
	}

	void ScrollBar::color(unsigned char r, unsigned char g, unsigned char b)
	{
		r_ = r;
		g_ = g;
		b_ = b;
	}

	SDL_Rect ScrollBar::rect() const
	{
		SDL_Rect r;
		float s = step(), v = val_ - min_;
		r.x = x_;
		r.y = y_ + v * s;
		r.w = w_;
		r.h = h_ * scale_;
		if(r.h < smallest_) r.h = smallest_;

		return r;
	}
	
	float ScrollBar::step() const
	{
		float d = max_ - min_;
		float s = (float) h_ * (1.0f - scale_) / d;
		if(h_ * scale_ < smallest_ && d > 1.0f) s = (float)(h_ - smallest_) / d;//(d - 1.0f);

		return s;
	}
		
	} // namespace tab
