	#include "sdl_font.h"
	
	namespace tab
	{
	
	// SDLFont
	
	SDL_Renderer* SDLFont::renderer_ = 0;

	void SDLFont::init(SDL_Renderer* renderer)
	{
		renderer_ = renderer;
	}
	
	SDLFont::~SDLFont()
	{
		clear();
	}
		
	void SDLFont::load(TTF_Font* ttf_font)
	{
		
		clear();
		
		static SDL_Color white = {255, 255, 255, 255};
		
		h_ = TTF_FontHeight(ttf_font);
		int adv = 0;
		
		for(int i = 0; i < 256; ++i)
		{
			char ch = TTF_GlyphIsProvided(ttf_font, i) ? i : ' ';
			TTF_GlyphMetrics(ttf_font, ch, 0, 0, 0, 0, &adv);
			SDL_Surface* glyph = TTF_RenderGlyph_Blended(ttf_font, ch, white);
			
			SDL_Texture* t = SDL_CreateTextureFromSurface(renderer_, glyph);
			tex_.push_back(t);
			adv_.push_back(adv);
			
			SDL_FreeSurface(glyph);
		}
	}

	void SDLFont::clear()
	{
		r_ = g_ = b_ = a_ = 255;
		for(SDL_Texture* t : tex_)
		{
			SDL_DestroyTexture(t);
		}
		tex_.clear();
		adv_.clear();
		h_ = 0;
		

	}
	
	void SDLFont::print(int x, int y, std::string text)
	{	
		SDL_Rect dest;
		dest.x = x;
		dest.y = y;
		dest.h = h_;
		
		for(char t : text)
		{
			SDL_SetTextureColorMod(tex_[t], r_, g_, b_);
			SDL_SetTextureAlphaMod(tex_[t], a_);
			dest.w = adv_[t];
			SDL_RenderCopy(renderer_, tex_[t], 0, &dest);
			dest.x += dest.w;
		}
	}
	

	void SDLFont::draw_rect(const SDL_Rect& r)
	{
		SDL_SetRenderDrawColor(renderer_, r_, g_, b_, a_);
		SDL_RenderFillRect(renderer_, &r);
		// SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
	}
	
	SDL_Rect SDLFont::text_size(std::string text) const
	{
		SDL_Rect ts;
		ts.x = 0;
		ts.y = 0;
		ts.w = 0;
		ts.h = h_;
		
		for(char t : text)
		{
			ts.w += adv_[t];
		}
		
		return ts;
	}
	
	} // namespace tab
