	#ifndef TAB_OPENGL_FONT_H
	#define TAB_OPENGL_FONT_H
	
	#include <SDL2/SDL_ttf.h>
	#include <SDL2/SDL_opengl.h>
	#include <SDL2/SDL.h>
	
	#include <vector>
	#include <string>
	
	#include "font.h"
	
	namespace tab
	{
	
	class OpenGLFont : public Font
	{
	public:
		~OpenGLFont();
		
		void load(TTF_Font* ttf_font);
		void clear();
		
		void print(int x, int y, std::string text);
		void draw_rect(const SDL_Rect& r);
		
		SDL_Rect text_size(std::string text) const;
	private:
		std::vector<GLuint> tex_;
		std::vector<unsigned> adv_;
		unsigned h_ = 0;
	};
	
	} // namespace tab
	
	
	
	
	#endif
