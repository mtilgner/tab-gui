
	#include "font.h"

	namespace tab
	{

	//		Font

	void Font::color(unsigned char r, unsigned char g, unsigned char b)
	{
		r_ = r;
		g_ = g;
		b_ = b;
	}

	void Font::alpha(unsigned char a)
	{
		a_ = a;
	}

	
	
	void print(Font* font, const SDL_Rect& dest, TextAlign align, std::string text)
	{
		int x = 0, y = 0;
		SDL_Rect ts = font->text_size(text);
		
		switch(align)
		{
			case ALIGN_LEFT:
				x = dest.x;
				break;
			
			case ALIGN_RIGHT:
				x = dest.x + dest.w - ts.w;
				break;
			
			case ALIGN_CENTER:
				x = dest.x + (dest.w - ts.w) / 2;
				break;
		}
		
		y = dest.y + (dest.h - ts.h) / 2;
		
		font->print(x, y, text);
	}
	
	} // namespace tab

