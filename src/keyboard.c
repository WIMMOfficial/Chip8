#include <chip8.h>

key keyboard[] =
{
  {0x1, SDLK_1},
  {0x2, SDLK_2},
  {0x3, SDLK_3},
  {0x4, SDLK_4},
  {0x5, SDLK_5},
  {0x6, SDLK_6},
  {0x7, SDLK_7},
  {0x8, SDLK_8},
  {0x9, SDLK_9},
  {0xA, SDLK_a},
  {0xB, SDLK_b},
  {0xC, SDLK_c},
  {0xD, SDLK_d},
  {0xE, SDLK_e},
  {0xF, SDLK_f}, 
};

SDLKey get_symbol(int keycode)
{
  for (int i = 0; i < 15; ++i)
    {
      if (keycode == keyboard[i].keycode)
	return keyboard[i].symbol;
    }
  return SDLK_END;
}

int get_keycode(SDLKey s)
{
  for (int i = 0; i < 15; ++i)
    {
      if (s == keyboard[i].symbol)
	return keyboard[i].keycode;
    }
  
  return -1;
}

bool key_pressed(int keycode)
{
  Uint8* key_status = SDL_GetKeyState(NULL);

  if (key_status[get_symbol(keycode)])
    return true;

  return false;
}


int get_key_pressed(void)
{
  int keycode = -1;

  while (keycode == -1)
    {
      if (SDL_PollEvent(&event))
		{
		  switch(event.type)
			{
			case SDL_KEYDOWN:
			  keycode = get_keycode(event.key.keysym.sym);
			  break;
			}
		}
      SDL_Delay(20);
    }
  return keycode;
}
