/*I N C L U D E S *****************************************************************************/

#include <chip8.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


/*F U N C T I O N S  *****************************************************************************/

void stack_push(Uint16 value)
{
  if (cpu.sp.WORD >= STACK_MAX_SIZE)
    {
      fprintf(stderr, "Error! Stack is overflow\n");
      exit(-1);
    }
	
  cpu.sp.WORD += 1;
  stack[cpu.sp.WORD].WORD = value;
}


Uint16 stack_pop(void)
{
  if (cpu.sp.WORD == 0)
    {
      fprintf(stderr, "Error! There is no elements on the stack\n");
      exit(-1);
    }

  Uint16 buf;
  
  buf = stack[cpu.sp.WORD].WORD;
  cpu.sp.WORD -= 1;
  return buf;
}


Uint32 timer_interrupt(Uint32 interval, void* params)
{
	decrement_timers = true;
	return interval;
}


int timer_init(void)
{
	int result = 1;
	SDL_InitSubSystem(SDL_INIT_TIMER);
	timer = SDL_AddTimer(17, timer_interrupt, NULL);
	
	if (timer == NULL)
	{
		result = 0;
		printf("Error! Coundn't create timer: %s\n ", SDL_GetError());
	}
	
	return result;
}


void initialize(void)
{
  int i = 0;
  
  cpu.pc.WORD = 0x0200;
  cpu.sp.WORD = 0;
  cpu.ip.WORD = 0;
  
  Uint8 arr_sprite[80] =
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
  
  for(i = 0; i < 4096; ++i)
    {
		if (i < 80)
			mem_write_byte(i, arr_sprite[i]);
		else
			mem_write_byte(i, 0);    
    }

  for (i = 0; i < 16; ++i)
    {
     cpu.V[i] = 0x0;
    }

  cpu.op_string = malloc(200);  
} 

void sdl_process_event(void)
{
	if (SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_QUIT:
				cpu.state = CPU_STOP;
			break;

			case SDL_KEYDOWN:
				if (event.key.keysym.sym == KEY_NORMAL_MODE)
					cpu.state = CPU_RUN;

				else if (event.key.keysym.sym == KEY_STOP)
					cpu.state = CPU_STOP;
				
				else if (event.key.keysym.sym == KEY_DEBUG_MODE)
					cpu.state = CPU_DEBUG;
					
				else if (event.key.keysym.sym == KEY_TRACE_MODE)
					cpu.state = CPU_TRACE;
					
				else if (event.key.keysym.sym == KEY_NEXT_COMMAND)
					cpu.state = CPU_STEP;

			break;
		}
	}
} 
    
void cycle(void)
{
  /*fetch*/
  /*decode*/
  /*execute*/
  /*check interrupts*/
  byte h, tmp_byte;
  word tmp_word;
  word tmp_ip;
  
  time_t t;
  
  int pixel, current_color, temp = 0;

  old_pc = cpu.pc.WORD;
  opcode.BYTE.high = mem_read_byte(cpu.pc.WORD);
  cpu.pc.WORD += 1;
  opcode.BYTE.low = mem_read_byte(cpu.pc.WORD);
  cpu.pc.WORD += 1;
  
  switch(opcode.BYTE.high & 0xF0)
  {
      
    case 0x00:
      switch(opcode.BYTE.low)
	{
		
/*Clean screen*/
	case 0xE0:
	   display_blank();
	   sprintf(cpu.op_string,"CLS");
	  break;
	  
	  
/*Return from subroutine*/
	case 0xEE:
	  cpu.pc.WORD = stack_pop();
	  sprintf(cpu.op_string, "RTS");
	  break;

	default:
	  fprintf(stderr, "Command not found! Opcode:%x\n", opcode.WORD);
	break;
	}
    break;
    
/*Jump to address */
    case 0x10:
      cpu.pc.WORD = (opcode.WORD & 0x0FFF);
	  sprintf(cpu.op_string,"JUMP %03X", cpu.pc.WORD);
      break;
      
/*Call subroutine*/
    case 0x20:
      stack_push(cpu.pc.WORD);
	  cpu.pc.WORD = (opcode.WORD & 0x0FFF);
	  sprintf(cpu.op_string,"CALL %03X", cpu.pc.WORD);
     break;
    
/*Skip the next insturction if condition is true*/
    case 0x30:
      
      if (cpu.V[X] == opcode.BYTE.low)
		{
		 cpu.pc.WORD += 2;
		}
		sprintf(cpu.op_string,"SKE V%X, %02X", X, opcode.BYTE.low);
      
      break;
	  
/*Skip the next insturction if condition is false*/
    case 0x40:
       
      if (cpu.V[X] != opcode.BYTE.low)
		{
		 cpu.pc.WORD += 2;
		}
		sprintf(cpu.op_string,"SKNE V%X, %02X", X, opcode.BYTE.low);
      
      break;
	  
/*Skip the next instruction if VX equals VY*/
    case 0x50:
       
      if (cpu.V[X] == cpu.V[Y])
		{
		 cpu.pc.WORD += 2;
		}
		sprintf(cpu.op_string,"SKE V%X, V%X", X, Y);
      
      break;
	  
/*Set VX to low byte*/
    case 0x60:
      cpu.V[X] = opcode.WORD & 0x00FF;
	  sprintf(cpu.op_string,"LOAD V%X, %02X", X, (opcode.WORD & 0x00FF));
      break;
	  
/*Add low byte to VX*/
    case 0x70:
      cpu.V[X] +=  opcode.WORD & 0x00FF;
	  sprintf(cpu.op_string,"ADD V%X, %02X", X, (opcode.WORD & 0x00FF));
      break;
    
    case 0x80:
      switch(opcode.BYTE.low & 0x0F)
	{
		
		/*Set VX to the value of VY. */
	case 0x0:
	  cpu.V[X] = cpu.V[Y];
	  sprintf(cpu.op_string,"LOAD V%X, V%X", X, Y);
	  break;
	  
	  /*Set VX to VX or VY */
	case 0x1:
	  cpu.V[X] |= cpu.V[Y];
	  sprintf(cpu.op_string,"OR V%X, V%X", X, Y);
	  break;
	  
	  /*Set VX to VX and VY */
	case 0x2:
	  cpu.V[X] &= cpu.V[Y];
	  sprintf(cpu.op_string,"AND V%X, V%X", X, Y);
	  break;

	 /*Set VX to VX xor VY */
	case 0x3:
	  cpu.V[X] ^= cpu.V[Y];
	  sprintf(cpu.op_string,"XOR V%X, V%X", X, Y);
	  break;

	 /*Add VY to VX. VF is set to 1 
	  * when there's a carry, and to 0 when there isn't. */
	case 0x4:

	   temp = cpu.V[X] + cpu.V[Y];
	   if (temp > 255)
	   {
		   cpu.V[0xF] = 1;
		   cpu.V[X] = temp - 256;
	   }
	   
	   else 
	   {
		   cpu.V[0xF] = 0;
		   cpu.V[X] = temp;
	   }
	   
	   sprintf(cpu.op_string,"ADD V%X, V%X", X, Y);
	  break;

	 /*VY is subtracted from VX. VF is set to 0 
	  * when there's a borrow, and 1 when there isn't*/
	case 0x5:   
	
	  if (cpu.V[X] > cpu.V[Y])
	  {
	    cpu.V[0xF] = 0x1;
		cpu.V[X] -= cpu.V[Y];
	  }

	  else
	  {
	    cpu.V[0xF] = 0x0;
		cpu.V[X] = 256 + cpu.V[X] - cpu.V[Y];
	  }
	  
	  sprintf(cpu.op_string,"SUB V%X, V%X", X, Y);
	  break;
	 
	 /*Store the least significant bit of VX in VF 
	  * and then shifts VX to the right by 1*/
	case 0x6:
	 if ((cpu.V[X] & 1))
			cpu.V[0xF] = 1;
	 else 
		 cpu.V[0xF] = 0;
	
	  cpu.V[X] >>= 1;
	  sprintf(cpu.op_string,"SHR V%X", X);  
	  break;
	
	/*Set VX to VY minus VX. VF is set to 0 
	 * when there's a borrow, and 1 when there isn't. */
	case 0x7:   
	  if (cpu.V[Y] < cpu.V[X])
	  {
	      cpu.V[0xF] = 1;
		  cpu.V[X] = cpu.V[Y] - cpu.V[X];
		  
	  }
	  else
	  {
	      cpu.V[0xF] = 0;
		  cpu.V[X] = 256 - cpu.V[Y] - cpu.V[X];
	  } 
	  
	  sprintf(cpu.op_string,"SUBN V%X, V%X", X, Y);
	  break;
	  
	/*Stores the most significant bit of VX in VF 
	* and then shifts VX to the left by 1*/
	case 0xE:
	  if (cpu.V[X] & 0x80)
			cpu.V[0xF] = 1;
	 else 
		 cpu.V[0xF] = 0;
	
	  cpu.V[X] <<= 1;
	  sprintf(cpu.op_string,"SHL V%X", X);  
	  break;

    default:
	  fprintf(stderr, "Command not found! Opcode:%x\n", opcode.WORD);
	break;
	
	}
	break;
    
/*Skip the next instruction if VX doesn't equal VY*/
    case 0x90:
	  if (cpu.V[X] != cpu.V[Y])
	  {
		  cpu.pc.WORD += 2;
	  }
	  sprintf(cpu.op_string,"SKNE V%X, V%X", X, Y);
      break;

/*Set ip to the address NNN */ 
    case 0xA0:
      cpu.ip.WORD = (opcode.WORD & 0x0FFF);
	  sprintf(cpu.op_string,"LOAD IP, %02X", (opcode.WORD & 0x0FFF));
      break;
	  
	  
/*Jump to the address NNN plus V0*/
    case 0xB0:
      cpu.pc.WORD = cpu.ip.WORD + (opcode.WORD & 0x0FFF);
	  sprintf(cpu.op_string,"JMP IP + %02X", (opcode.WORD & 0x0FFF));
      break;

/*Set VX to the result of a bitwise and operation on a random number (0 to 255) and NN. */
    case 0xC0:
      srand((unsigned)time(&t));
      cpu.V[X] = (rand() % 255) & opcode.BYTE.low;
	  sprintf(cpu.op_string,"RAND V%X, %X", X, (opcode.BYTE.low));
      break;

/*Draw a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of h pixels*/
    case 0xD0:
      h = opcode.BYTE.low & 0xF;
	  tmp_word.WORD = cpu.ip.WORD;
      cpu.V[0xF] = 0;
	  int coord_x, coord_y;
	  
     
	  for (int i = 0; i < h; ++i)
	  {
		tmp_byte = mem_read_byte(cpu.ip.WORD + i);
		coord_y = cpu.V[Y] + i;
		coord_y %= DISPLAY_HEIGHT;

		for (int j = 0; j < 8; ++j)
		{
			coord_x = cpu.V[X] + j;
			coord_x %= DISPLAY_WIDTH;

			if (tmp_byte & 0x80)
				pixel = 1;

			else 
				pixel = 0;
			
			current_color = check_pixel(coord_x, coord_y);
			if (current_color && pixel)
				cpu.V[0xF] = 1;

			else
				cpu.V[0xF] = cpu.V[0xF];

			sprintf(cpu.op_string,"(DR) cur_col: %X, pixel: %X byte: %02X", current_color, pixel, tmp_byte);
			pixel ^= current_color;
			display_draw(coord_x, coord_y, pixel);
			
			tmp_byte <<= 1; 
		}
		 //sprintf(cpu.op_string,"DRAW V%X, V%X %X", X, Y, h);
	  }
	  if ((cpu.state != CPU_DEBUG) || cpu.state != CPU_TRACE)
		  display_refresh(0);
	  
      break;

    case 0xE0:
      switch(opcode.BYTE.low)
	{
		/*Skip the next instruction if the key stored in VX is pressed*/
	 case 0x9E:
	   if (key_pressed(cpu.V[X]))
	     {
	       cpu.pc.WORD += 2;
	     }
		 sprintf(cpu.op_string,"SKPR V%X", X);
	   break;

		/*Skip the next instruction if the key stored in VX isn't pressed */
	 case 0xA1:
	   if (!key_pressed(cpu.V[X]))
	     {
	       cpu.pc.WORD += 2;
	     }
		 sprintf(cpu.op_string,"SKUP V%X", X);
	   break;
	   
	   default:
		fprintf(stderr, "Command not found! Opcode:%x\n", opcode.WORD);
	   break;
	}
      break;

    case 0xF0:
      switch(opcode.BYTE.low)
	{
		/*Set VX to the value of the delay timer */
	case 0x07:
	  cpu.V[X] = cpu.dt;
	  sprintf(cpu.op_string,"LOAD V%X, DELAY %X", X, cpu.dt);
	      break;

		/*A key press is awaited, and then stored in VX */
	 case 0x0A:
	    cpu.V[X] = get_key_pressed();
		sprintf(cpu.op_string,"KEY V%X", X);
	    break;
	    
	  break;
	  
		/*Set the delay timer to VX*/
	case 0x15:
		cpu.dt = cpu.V[X];
		sprintf(cpu.op_string,"LOAD DELAY %X", cpu.V[X]);
	  break;

		/*Set the sound timer to VX */
	case 0x18:
	    cpu.st = cpu.V[X]; 
		sprintf(cpu.op_string,"LOAD SOUND %X", cpu.V[X]);
	   break;

		/*Add VX to ip*/
	case 0x1E:
	    cpu.ip.WORD += cpu.V[X];
		sprintf(cpu.op_string,"ADD IP %X", cpu.V[X]);
	  break;

		/*Set ip to the location of the sprite for the character in VX */
	case 0x29:
	  cpu.ip.WORD = cpu.V[X] * 5;
	  sprintf(cpu.op_string,"LOAD IP %X", cpu.V[X]);
	  break;

		/*Store the binary-coded decimal representation of VX*/
	case 0x33:
	
	tmp_word.WORD = cpu.V[X] / 100;
	tmp_ip.WORD = cpu.ip.WORD;
	mem_write_word(tmp_ip.WORD, tmp_word.WORD);
	
	tmp_ip.WORD += 1;
	tmp_word.WORD = (cpu.V[X] / 10) % 10;
	mem_write_word(tmp_ip.WORD, tmp_word.WORD);
	
	
	tmp_ip.WORD += 1;
	tmp_word.WORD = cpu.V[X] % 10;
	mem_write_word(tmp_ip.WORD, tmp_word.WORD);
	sprintf(cpu.op_string,"BCD V%X (%03d)", X, cpu.V[X]);
	break;

		/*Store V0 to VX (including VX) in memory starting at address ip*/
	case 0x55:
	  reg_dump(X);
	  sprintf(cpu.op_string,"STOR %X", X);
	  break;


		/*Fill V0 to VX (including VX) with values from memory starting at address */
	case 0x65:
	  reg_load(X);
	  sprintf(cpu.op_string,"LOAD %X", X);
	  break;
	}
	
      break;
	  
	  default:
	   fprintf(stderr, "Command not found! Opcode:%x\n", opcode.WORD);
	  break;
   }
}


void execute()
{
	 while(cpu.state != CPU_STOP)
	 {
		 cycle();
		 
		 if ((cpu.state == CPU_DEBUG) || cpu.state == CPU_TRACE)
		 {
			display_refresh(1);
			while (cpu.state == CPU_DEBUG)
			{
				sdl_process_event();
				SDL_Delay(20);
			}
			
			if (cpu.state == CPU_STEP)
			{
				cpu.state = CPU_DEBUG; 
			}
			
		 }
		 
		 else
		{
			SDL_Delay(delay);
		}
		
		 if (decrement_timers)
		 {
			 if (cpu.dt > 0)
				 cpu.dt -= 1;

			if (cpu.st > 0)
				cpu.st -= 1;
			decrement_timers = false;
		 }
		 
		 //SDL_Delay(delay);
		 sdl_process_event();
		
	 }
}


/*E N D    O F   F I L E *****************************************************************************/