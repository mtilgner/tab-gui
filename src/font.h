	#ifndef TAB_FONT_H
	#define TAB_FONT_H
	
	#include <SDL2/SDL.h>
	#include <string>

	namespace tab
	{
	
	enum TextAlign
	{
		ALIGN_LEFT,
		ALIGN_RIGHT,
		ALIGN_CENTER
	};
		
	class Font
	{
	public:
		virtual ~Font(){}

		virtual void print(int x, int y, std::string text) = 0;
		virtual void draw_rect(const SDL_Rect& r) = 0;

		virtual SDL_Rect text_size(std::string text) const = 0;


		void color(unsigned char r, unsigned char g, unsigned char b);
		void alpha(unsigned char a);
	protected:
		unsigned char r_ = 255, g_ = 255, b_= 255, a_ = 255;
	};
	

//	void load_ttf(Font* font, TTF_Font* ttf_font);
	
	void print(Font* font, const SDL_Rect& dest, TextAlign align, std::string text);

	} // namespace tab



	#endif
