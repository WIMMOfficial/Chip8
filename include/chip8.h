#pragma once

#include <SDL/SDL.h>
#include <stdbool.h>

#define STACK_MAX_SIZE 256
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define SCALE 14
#define X opcode.WORD & 0x0F00 >> 8
#define Y opcode.WORD & 0x00F0 >> 4

typedef unsigned char byte;

typedef union
{
  unsigned short WORD;
  
  struct
  {
    byte low, high;
    
  }BYTE;
  
}word;


typedef struct
{
  int keycode;
  SDLKey symbol;
  
}key;


typedef struct
{
  
  word V[16]; //General purpose registers  
  word pc, ip, sp;    //programm counter, instruction ptr, stack ptr
  byte dt, st;       //delay timer, sound timer
  
}chip8;

/* G L O B A L S ********************************************/

/*CPU*/
chip8 cpu;
word stack[STACK_MAX_SIZE], opcode;
byte memory[4096];

/*Display*/
int display_width, display_height, scale;
SDL_Surface* screen; // main screen
SDL_Surface* overlay;
SDL_Surface* virtscreen; // Chip8 virtual screen


/*Colors*/
Uint32 COLOR_BLACK;
Uint32 COLOR_WHITE;
Uint32 COLOR_DGREEN;
Uint32 COLOR_LGREEN;
SDL_Color COLOR_TEXT;


SDL_Event event;

/*array of sprites*/
unsigned char arr_sprite[80] =
  {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80, // F 
  };


/*P R O T O T Y P E S ***********************************/

/*chip8.c*/
void initialize(void); // initialize the cpu
void cycle(void);
void stack_push(unsigned short);
unsigned short stack_pop(void);


/*graphics.c*/
void init_colors(SDL_Surface*);
int check_pixel(int, int);
void draw(int, int, int);
void display_flush(void);
void blit_surface(SDL_Surface*, SDL_Surface, int, int);
SDL_Surface* display_message(char*, SDL_Color);
void display_trace_message(void);
void dispay_refresh(int);
SDL_Surface* surface_create(int, int, int Uint32);
bool display_init(void);
void surface_destroy(void);


/*keyboard.c*/
SDLKey get_symbol(int);
int get_keycode(SDLKey);
bool key_pressed(int);
int get_key_pressed(void);

/*memory.c*/
void reg_dump(byte);
void reg_load(byte);
byte mem_read(unsigned short);
void mem_write(unsigned short, byte);



