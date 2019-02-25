	#ifndef TAB_SDL_FONT_H
	#define TAB_SDL_FONT_H
	
	#include <SDL2/SDL_ttf.h>
	#include <SDL2/SDL.h>
	
	#include <vector>
	#include <string>
	
	#include "font.h"
	
	namespace tab
	{
	
	class SDLFont : public Font
	{
	public:
		static void init(SDL_Renderer* renderer);
		
		~SDLFont();
		
		void load(TTF_Font* ttf_font);
		void clear();

		void print(int x, int y, std::string text);
		void draw_rect(const SDL_Rect& r);

		SDL_Rect text_size(std::string text) const;
	private:
		std::vector<SDL_Texture*> tex_;
		std::vector<unsigned> adv_;
		unsigned h_ = 0;
		static SDL_Renderer* renderer_;
	};
	
	
	} // namespace tab
	
	
	
	
	
	#endif
	
