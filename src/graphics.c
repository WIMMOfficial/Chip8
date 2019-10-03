/*I N C LU D E S ***************************************************************************/

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <stdlib.h>
#include <chip8.h>
#include <stdbool.h>

/*L O C A L S *****************************************************************************/

TTF_Font *message_font;



/*F U N C T I O N S  *****************************************************************************/

void init_colors(SDL_Surface* surface)
{
	COLOR_BLACK = SDL_MapRGB(surface->format, 0, 0, 0);
	COLOR_WHITE = SDL_MapRGB(surface->format, 250, 250, 250);
	COLOR_DGREEN = SDL_MapRGB(surface->format, 0, 70, 0);
	COLOR_LGREEN = SDL_MapRGB(surface->format, 0, 200, 0);
	COLOR_TEXT.r = 255;
	COLOR_TEXT.g = 255;
	COLOR_TEXT.b = 255;
}

void display_blank(void)
{
	display_flush(virtscreen, COLOR_BLACK);
}

/*Check a pixel. If it's turned on return 1, else 0 */
uint16_t check_pixel(int x, int y)
{
	uint8_t r, g, b;
	uint32_t color = 0;
	uint16_t pixel_color = 0;
	
	x *= scale;
	y *= scale;
	uint32_t *pixels = (uint32_t*)virtscreen->pixels; // pixels from virt_screen
	uint32_t pixel = pixels[(virtscreen->w * y) + x]; 
	SDL_GetRGB(pixel, virtscreen->format, &r, &g, &b); // get RGB values from a pixel
	color = SDL_MapRGB(virtscreen->format, r, g, b);
	
	if (color == COLOR_WHITE)
	{
		pixel_color = 1;
	}	
	
	return pixel_color;
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




void display_flush(SDL_Surface* surf, uint32_t color)
{
	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = display_width;
	rect.h = display_height;
	SDL_FillRect(surf, &rect, color);
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
	
	sprintf(buffer, "ir:%04X dt:%02X st:%02X pc:%04X %04X %s",
			cpu.ir.WORD, cpu.dt, cpu.st, old_pc, opcode.WORD, cpu.op_string);
			
	msg = display_message(buffer, COLOR_TEXT);
	blit_surface(msg, overlay, 10, display_height - 53);
	SDL_FreeSurface(msg);
	
	sprintf(buffer, "V0:%02X V1:%02X V2:%02X V3:%02X V4:%02X V5:%02X V6:%02X V7:%02X",
			 cpu.V[0], cpu.V[1], cpu.V[2], cpu.V[3], 
			 cpu.V[4], cpu.V[5], cpu.V[6], cpu.V[7]);
			 
	msg = display_message(buffer,COLOR_TEXT);
	blit_surface(msg,overlay, 10, display_height - 38);
	SDL_FreeSurface(msg);
	
	sprintf(buffer, "V8:%02X V9:%02X VA:%02X VB:%02X VC:%02X VD:%02X VE:%02X VF:%02X",
			 cpu.V[8], cpu.V[9], cpu.V[10], cpu.V[11], 
			 cpu.V[12], cpu.V[13], cpu.V[14], cpu.V[15]);
			 
	msg = display_message(buffer,COLOR_TEXT);
	blit_surface(msg, overlay, 10, display_height - 23);
	SDL_FreeSurface(msg);
	
	free(buffer);
}


void display_refresh(bool overlay_on)
{
	blit_surface(virtscreen, screen, 0, 0);
	if (overlay_on)
	{
		display_trace_message();
		blit_surface(overlay, screen, 0,0);
	}
	display_flush(overlay, COLOR_BLACK);
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

void display_draw(int x, int y, int color)
{
	SDL_Rect pixel;
	uint32_t pixel_color = COLOR_BLACK;
	
	pixel.x = x * scale;
	pixel.y = y * scale;
	pixel.w = scale;
	pixel.h = scale;
	
	if (color)
	{
		pixel_color = COLOR_WHITE;
	}
	
	SDL_FillRect(virtscreen, &pixel, pixel_color);
}


SDL_Surface* surface_create(int widht, int height, int alpha, uint32_t color_key)
{
	uint32_t rmask, gmask, bmask, amask;
	SDL_Surface *tempsurface, *newsurface;
	
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
	
	tempsurface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA,  display_width, display_height, 32, rmask, gmask, bmask, amask);
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
		
		display_flush(newsurface, COLOR_BLACK);
	}
	
	return newsurface;
}


int display_init(void)
{
	int8_t res = 0;
	
	TTF_Init();
	
	message_font = TTF_OpenFont("VeraMono.ttf", 11);
	if (!message_font)
		exit(-1);
	
	display_width = DISPLAY_WIDTH * scale;
	display_height = DISPLAY_HEIGHT * scale;
	
	screen = SDL_SetVideoMode(display_width, display_height, 32, SDL_SWSURFACE);
	
	if (screen == NULL)
	{
		printf("Error! Unable to set videomode\n");
	}
	else
	{
		SDL_SetAlpha(screen, SDL_SRCALPHA, 255);
		SDL_WM_SetCaption("Chip8 Emulator", NULL);
		init_colors(screen);
		virtscreen = surface_create(display_width, display_height, 255, -1);
		overlay = surface_create(display_width, display_height, 200, COLOR_BLACK);
		
		res = 1;
	}
	return ((virtscreen != NULL) && (overlay != NULL) && res);
}


void display_destroy(void)
{
	SDL_FreeSurface(screen);
	SDL_FreeSurface(overlay);
	SDL_FreeSurface(virtscreen);
}

/*E N D    O F    F I L E *****************************************************************************/