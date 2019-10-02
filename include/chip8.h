#pragma once

/*I N C L U D E S *****************************************************************************/

#include <SDL/SDL.h>
#include <stdbool.h>

/*D E F I N E S ********************************************************************************/

#define STACK_MAX_SIZE 256

#define KEY_NORMAL_MODE SDLK_F12
#define KEY_STOP SDLK_ESCAPE
#define KEY_DEBUG_MODE SDLK_F1
#define KEY_TRACE_MODE SDLK_F2
#define KEY_NEXT_COMMAND SDLK_RIGHT 


#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define SCALE 15

#define X (opcode.WORD & 0x0F00) >> 8
#define Y (opcode.WORD & 0x00F0) >> 4



#define CPU_RUN 1
#define CPU_PAUSE 2
#define CPU_DEBUG 3
#define CPU_TRACE 4
#define CPU_STEP 5
#define CPU_STOP 6

/*S T R U C T S *****************************************************************************/

typedef unsigned char byte;

typedef union
{
  unsigned short int WORD;
  
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
  
  byte V[16];          //General purpose registers  
  word pc, ip, sp;    //Programm counter, instruction ptr, stack ptr
  byte dt, st;       //Delay timer, sound timer
  int state;        //The current state of cpu
  char* op_string;
  
}chip8;

/* G L O B A L S *******************************************************************************/

/*CPU*/
chip8 cpu;
word stack[STACK_MAX_SIZE], opcode;
byte memory[4096];
SDL_TimerID timer;
bool decrement_timers;
int delay;
Uint16 old_pc; 


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



/*P R O T O T Y P E S **********************************************************************/

/*chip8.c*/
void initialize(void); // initialize the cpu
void cycle(void);
Uint32 timer_interrupt(Uint32, void*);
int timer_init(void);
void execute();
void sdl_process_evnet();
void stack_push(Uint16);
Uint16 stack_pop(void);


/*graphics.c*/
void init_colors(SDL_Surface*);
int check_pixel(int, int);
void display_blank(void);
void display_draw(int, int, int);
void display_flush(SDL_Surface*, Uint32);
void blit_surface(SDL_Surface*, SDL_Surface*, int, int);
SDL_Surface* display_message(char*, SDL_Color);
void display_trace_message(void);
void display_refresh(int);
int display_init(void);
void surface_destroy(void);
void display_destroy(void);


/*keyboard.c*/
SDLKey get_symbol(int);
int get_keycode(SDLKey);
bool key_pressed(int);
int get_key_pressed(void);

/*memory.c*/
void reg_dump(byte);
void reg_load(byte);
byte mem_read_byte(Uint16);
void mem_write_byte(Uint16, byte);
Uint16 mem_read_word(Uint16);
void mem_write_word(Uint16, Uint16);


/*graphics_test.c*/
void test_display_setup(void);
void test_display_destroy(void);
void test_display_get_pixel(void);
void test_display_pixel_turned_off(void);
void test_display_blank(void);


/*chip8_test.c*/
void test_chip8_jump_to_address(void);
void test_chip8_jump_to_subroutine(void);
void test_chip8_skip_next_instr_if_equal(void);
void test_chip8_skip_next_instr_if_not_equal(void);
void test_chip8_skip_next_instr_if_registers_are_equal(void);
void test_chip8_move_register_to_register(void);
void test_chip8_add_registers(void);
void test_chip8_set_register_to_val(void);
void test_chip8_or_registers(void);
void test_chip8_and_registers(void);
void test_chip8_xor_registers(void);
void test_chip8_substr_registers(void);
void test_chip8_shifts(void);
void test_chip8_reg_dump(void);
void test_chip8_reg_load(void);

/*test.c*/
int tests(void);

/*E N D   O F  F I L E  *****************************************************************************/