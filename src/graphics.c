#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <stdlib.h>
#include <chip8.h>
#include <stdbool.h>


TTF_Font *message_font;

void init_colors(SDL_Surface* surface)
{
	COLOR_BLACK = SDL_MapRGB(surface->format, 0, 0, 0);
	COLOR_WHITE = SDL_MapRGB(surface->format, 250, 250, 250);
	COLOR_DGREEN = SDL_MapRGB(surface->format, 0, 70, 0);
	COLOR_LGREEN = SDL_MapRGB(surface->format, 0, 200,0);
	COLOR_TEXT.r = 255;
	COLOR_TEXT.g = 255;
	COLOR_TEXT.b = 255;
}


/*Check a pixel. If it's turned on return 1, else 0 */
int check_pixel(int x, int y)
{
	Uint8 r, g, b;
	Uint32 color = 0;
	x *= scale;
	y *= scale;
	//Uint32 *pixels = (Uin32*)screen->pixels; // create an array of pixels from virt_screen
	Uint32 pixel = (Uint32*)virtscreen->pixels[(virtscreen->w * y) + x]; // ?????????? DEBUG!!!!!
	SDL_GetRGB(pixel, screen->format, &r, &g, &b); // get RGB values from a pixel
	color = SDL_MapRGB(screen->format, r, g, b);
	
	if (color == COLOR_WHITE)
	{
		return 1;
	}	
	
	return 0;
}


void blit_surface(SDL_Surface* src, SDL_Surface* dst, int x, int y)
{
	SDL_Rect location;
	location.x = x;
	location.y = y;
	SDL_BlitSurface(src, NULL, dst, &location);
}



SDL_Surface* display_message(char *text, SDL_Color color)
{
	SDL_Surface* msg;
	msg = TTF_RenderText_Solid(message_font, text, color);
	return msg;
}




void display_flush()
{
	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = display_width;
	rect.h = display_height;
	SDL_FillRect(screen, &rect, COLOR_BLACK);
}


void display_trace_message(void)
{
	SDL_Surface *msg;
	SDL_Rect box;
	
	box.x = 5;
	box.y = display_height - 58;
	box.w = 342;
	box.h = 53;
	SDL_FillRect(overlay, &box, COLOR_LGREEN);
	
	box.x = 6;
	box.y = display_height - 57;
	box.w = 340;
	box.h = 51;
	SDL_FillRect(overlay, &box, COLOR_DGREEN);
	
	char *buffer = (char*)malloc(200);
	
	fprintf(buffer, "ip:%04X dt:%02X st:%02X pc:%04X",
			cpu.ip.WORD, cpu.dt, cpu.st, cpu.pc.WORD);
			
	msg = display_message(buffer, COLOR_TEXT);
	blit_surface(msg, overlay, 10, display_height - 53);
	SDL_FreeSurface(msg);
	
	fprintf(buffer, "V0:%04X V1:%04X V2:%04X V3:%04X V4:%04X V5:%04X V6:%04X V7:%04",
			 cpu.V[0].WORD, cpu.V[1].WORD, cpu.V[2].WORD, cpu.V[3].WORD, 
			 cpu.V[4].WORD, cpu.V[5].WORD, cpu.V[6].WORD, cpu.V[7].WORD);
			 
	msg = display_message(buffer,COLOR_TEXT);
	blit_surface(msg,overlay, 10, display_height - 38);
	SDL_FreeSurface(msg);
	
	fprintf(buffer, "V8:%04X V9:%04X VA:%04X VB:%04X VC:%04X VD:%04X VE:%04X VF:%04",
			 cpu.V[8].WORD, cpu.V[9].WORD, cpu.V[10].WORD, cpu.V[11].WORD, 
			 cpu.V[12].WORD, cpu.V[13].WORD, cpu.V[14].WORD, cpu.V[15].WORD);
			 
	msg = display_message(buffer,COLOR_TEXT);
	blit_surface(msg, overlay, 10, display_height - 38);
	SDL_FreeSurface(msg);
	
	free(buffer);
}


void display_refresh(int overlay_on)
{
	blit_surface(virtscreen, screen, 0, 0);
	if (overlay_on)
	{
		display_trace_message();
		blit_surface(overlay, screen, 0,0);
	}
	display_flush();
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

void draw(int x, int y, int color)
{
	SDL_Rect pixel;
	Uint32 pixel_color = COLOR_BLACK;
	
	pixel.x = x *scale;
	pixel.y = y * scale;
	pixel.w = scale;
	pixel.h = scale;
	
	if (color)
	{
		pixel_color = COLOR_WHITE;
	}
	
	SDL_FillRect(virtscreen, &pixel, pixel_color);
}


SDL_Surface* surface_create(int widht, int height, int alpha, Uint32 color_key)
{
	Uint32 rmask, gmask, bmask, amask;
	SDL_Surface *tempsurface, *newsurface;
	
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
	
	tempsurface = SDL_CreateRGBSurface(0, display_width, display_height, 32, rmask, gmask, bmask, amask);
	newsurface = SDL_DisplayFormat(tempsurface);
	SDL_FreeSurface(tempsurface);
	
	if (newsurface == NULL)
	{
		fprintf(stderr, "Error! Surface is not created.\n");
	}
	else
	{
		SDL_SetAlpha(newsurface, SDL_SRCALPHA, alpha);
		
		if (color_key != -1)
		{
			SDL_SetColorKey(newsurface, SDL_SRCCOLORKEY, color_key);
		}
		
		display_flush();
	}
	
	return newsurface;
}


bool display_init(void)
{
	bool res = false;
	
	TTF_Init();
	
	message_font = TTF_OpenFont("VeraMono.ttf", 11);
	
	display_width = DISPLAY_WIDTH * scale;
	display_height = DISPLAY_HEIGHT * scale;
	
	screen = SDL_SetVideoMode(display_width, display_height, 32, SDL_SWSURFACE);
	
	if (screen == NULL)
	{
		fprintf(screen, "Error! Unable to set video mode");
		eixt(-1);
	}
	
	SDL_SetAlpha(screen, SDL_SRCALPHA, 255);
	SDL_WM_SetCaption("Chip8 Emulator", NULL);
	init_colors(screen);
	virtscreen = surface_create(display_widht, display_height, 255, -1);
	overlay = surface_create(display_width, display_height, 200, COLOR_BLACK);
	
	res = true;
	
	return ((virtscreen != NULL) && (overlay != NULL) & res);
}


void surface_destroy(void)
{
	SDL_FreeSurface(screen);
	SDL_FreeSurface(overlay);
	SDL_FreeSurface(virtscreen);
}

