	#ifndef TAB_SCROLLBAR_H
	#define TAB_SCROLLBAR_H

	#include <SDL2/SDL.h>

	namespace tab
	{
	
	class Font;

	class ScrollBar
	{
	public:
		void size(int w, int h);
		void position(int x, int y);
		void range(float min, float max);
		void scale(float s);
		void smallest(int s);

		float value() const;
		void set_value(float val);

		void handle(const SDL_Event* event);
		void draw(Font* font) const;
		void color(unsigned char r, unsigned char g, unsigned char b);
		SDL_Rect rect() const;
		
	private:
		int
		x_ = 0,
		y_ = 0,
		w_ = 0,
		h_ = 0,
		smallest_ = 0
		;

		float
		min_ = 0.0f,
		max_ = 0.0f,
		val_ = 0.0f
		;

		unsigned char
		r_ = 255,
		g_ = 255,
		b_ = 255
		;

		float scale_ = 0.5f;
		float step() const;
		
		static ScrollBar* active_scrollbar_;
		static int scroll_rel_drag_offs_;
	};


	} // namespace tab



	#endif
