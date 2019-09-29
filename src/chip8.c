#include <chip8.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void stack_push(unsigned short value)
{
  if (cpu.sp.WORD >= STACK_MAX_SIZE)
    {
      fprintf(stderr, "Error! Stack is overflow\n");
      exit(1);
    }
  
  stack[cpu.sp.WORD].WORD = value;
  cpu.sp.WORD++;
}


unsigned short stack_pop(void)
{
  if (cpu.sp.WORD == 0)
    {
      fprintf(stderr, "Error! There is no elements on the stack\n");
      exit(2);
    }
	
  word buf;
  
  buf.WORD = stack[cpu.sp.WORD].WORD;
  cpu.sp.WORD--;
  return buf.WORD;
}


void initialize(void)
{
  int i = 0;
  
  cpu.pc.WORD = 0x200;
  cpu.sp.WORD = 0;
  
  for(i = 0; i < 4096; ++i)
    {
      if (i < 80)
	{
	  memory[i] = arr_sprite[i];
	}
      
     else  
      memory[i] = 0;     
    }

  for (i = 0; i < 16; ++i)
    {
     cpu.V[i].WORD = 0;
    }

  display_flush();
  
}  
    
void cycle(void)
{
  /*fetch*/
  /*decode*/
  /*execute*/
  /*check interrupts*/

  byte tmp_byte;
  word tmp_word;
  
  time_t t;
  
  int pixel;
  int current_color;
  byte h;
  
  opcode.BYTE.high = mem_read(cpu.pc.WORD);
  cpu.pc.WORD++;
  opcode.BYTE.low = mem_read(cpu.pc.WORD);
  cpu.pc.WORD++;

  switch(opcode.WORD & 0xF000)
  {
      
    case 0x0000:
      switch(opcode.WORD & 0x00FF)
	{
		
/*Clean screen*/
	case 0x00E0:
	   display_flush();
	  break;
	  
	  
/*Return from subroutine*/
	case 0x00EE:
	  cpu.pc.WORD = stack_pop();
	  break;

	default:
	  fprintf(stderr, "Command not found! Opcode:%x\n", opcode.WORD);
	break;
	}
    break;
    
/*Jump to address */
    case 0x1000:
      cpu.pc.WORD = opcode.WORD & 0x0FFF;
      break;
      
/*Call subroutine*/
    case 0x2000:
      stack_push(cpu.pc.WORD);
	  cpu.pc.WORD = opcode.WORD & 0x0FFF;
     break;
    
/*Skip the next insturction if condition is true*/
    case 0x3000:
      
      if (cpu.V[X].BYTE.low == (opcode.WORD & 0x00FF))
		{
		 cpu.pc.WORD += 2;
		}
      
      break;
	  
/*Skip the next insturction if condition is false*/
    case 0x4000:
       
      if (cpu.V[X].BYTE.low != (opcode.WORD & 0x00FF))
		{
		 cpu.pc.WORD += 2;
		}
      
      break;
	  
/*Skip the next instruction if VX equals VY*/
    case 0x5000:
       
      if (cpu.V[X].WORD == cpu.V[Y].WORD)
		{
		 cpu.pc.WORD += 2;
		}
      
      break;
	  
/*Set VX to low byte*/
    case 0x6000:
      cpu.V[X].WORD = opcode.WORD & 0x00FF;
      break;
	  
/*Add low byte to VX*/
    case 0x7000:
      cpu.V[X].WORD +=  opcode.WORD & 0x00FF;
      break;
    
    case 0x8000:
      switch(opcode.WORD & 0x000F)
	{
		
		/*Set VX to the value of VY. */
	case 0x0000:
	  cpu.V[X].WORD = cpu.V[Y].WORD;
	  break;
	  
	  /*Set VX to VX or VY */
	case 0x0001:
	  cpu.V[X].WORD |= cpu.V[Y].WORD;
	  break;
	  
	  /*Set VX to VX and VY */
	case 0x0002:
	  cpu.V[X].WORD &= cpu.V[Y].WORD;
	  break;

	 /*Set VX to VX xor VY */
	case 0x0003:
	  cpu.V[X].WORD ^= cpu.V[Y].WORD;
	  break;

	 /*Add VY to VX. VF is set to 1 
	  * when there's a carry, and to 0 when there isn't. */
	case 0x0004:
	  if ((cpu.V[X].WORD + cpu.V[Y].WORD) > 255)
	    cpu.V[0xF].BYTE.low = 1;
	  
	  else
	    cpu.V[0xF].BYTE.low = 0;

	  cpu.V[X].WORD += cpu.V[Y].WORD;
	  break;

	 /*VY is subtracted from VX. VF is set to 0 
	  * when there's a borrow, and 1 when there isn't*/
	case 0x0005:   
	
	  if ((cpu.V[X].WORD - cpu.V[Y].WORD) < 0)
	    cpu.V[0xF].BYTE.low = 1;
	  else
	    cpu.V[0xF].BYTE.low = 0;

	  cpu.V[X].WORD -= cpu.V[Y].WORD;
	  break;
	 
	 /*Store the least significant bit of VX in VF 
	  * and then shifts VX to the right by 1*/
	case 0x0006:
	  cpu.V[0xF].BYTE.low = cpu.V[X].BYTE.low & 1;
	  cpu.V[X].WORD >>= 1;  
	  break;
	
	/*Set VX to VY minus VX. VF is set to 0 
	 * when there's a borrow, and 1 when there isn't. */
	case 0x0007:   
	  if ((cpu.V[Y].WORD - cpu.V[X].WORD) < 0)
	      cpu.V[0xF].BYTE.low = 1;
	  
	  else
	      cpu.V[0xF].BYTE.low = 0;
	    
	  cpu.V[X].WORD = cpu.V[Y].WORD - cpu.V[X].WORD;
	  break;
	  
	/*Stores the most significant bit of VX in VF 
	* and then shifts VX to the left by 1*/
	case 0x000E:
	  cpu.V[0xF].BYTE.low = cpu.V[X].BYTE.high & 0x8;
	  cpu.V[X].WORD <<= 1; 
	  break;
	  
	default:
	  fprintf(stderr, "Command not found! Opcode:%x\n", opcode.WORD);
	break;
	}
      
    break;

    default:
	  fprintf(stderr, "Command not found! Opcode:%x\n", opcode.WORD);
	break;
    
/*Skip the next instruction if VX doesn't equal VY*/
    case 0x9000:
	  if (cpu.V[X].WORD != cpu.V[Y].WORD)
	  {
		  cpu.pc.WORD += 2;
	  }
      break;

/*Set ip to the address NNN */ 
    case 0xA000:
      cpu.ip.WORD = memory[opcode.WORD & 0x0FFF];
      break;
	  
	  
/*Jump to the address NNN plus V0*/
    case 0xB000:
      cpu.pc.WORD = cpu.V[0].WORD + (opcode.WORD & 0x0FFF);
      break;

/*Set VX to the result of a bitwise and operation on a random number (0 to 255) and NN. */
    case 0xC000:
      srand((unsigned)time(&t));
      cpu.V[X].WORD = (rand()% 255 + 1) & (opcode.WORD & 0x00FF);
      break;

/*Draw a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of h pixels*/
    case 0xD000:
      h = opcode.WORD & 0x000F;
	  tmp_word.WORD = cpu.ip.WORD;
      cpu.V[0xF].BYTE.low = 0;
	  int coord_x, coord_y;
	  
     
	  for (int i = 0; i < h; ++i)
	  {
		tmp_byte = mem_read(cpu.ip.WORD + i);
		coord_y = cpu.V[Y].WORD + i;
		coord_y %= DISPLAY_HEIGHT;

		for (int j = 0; j < 8; ++j)
		{
			coord_x = cpu.V[X].WORD + j;
			coord_x %= DISPLAY_WIDTH;

			if (tmp_byte & 0x80 == 1)
				pixel = 1;

			else 
				pixel = 0;
			
			current_color = check_pixel(coord_x, coord_y);
			if (current_color == pixel)
				cpu.V[0xF].BYTE.low = 1;

			else
				cpu.V[0xF].BYTE.low = 0;

			pixel ^= current_color;
			draw(coord_x, coord_y, pixel);
			
			tmp_byte <<= 1; 
		}
	  }
	  
      break;

    case 0xE000:
      switch(opcode.WORD & 0x00FF)
	{
		/*Skip the next instruction if the key stored in VX is pressed*/
	 case 0x009E:
	   if (get_key_pressed() == cpu.V[X].WORD)
	     {
	       cpu.pc.WORD += 2;
	     }
	   break;

		/*Skip the next instruction if the key stored in VX isn't pressed */
	 case 0x00A1:
	   if (get_key_pressed() != cpu.V[X].WORD)
	     {
	       cpu.pc.WORD += 2;
	     }
	   break;
	}
      break;

		
    case 0xF000:
      switch(opcode.WORD & 0x00FF)
	{
		/*Set VX to the value of the delay timer */
	case 0x0007:
	  cpu.V[X].WORD = cpu.dt;
	      break;

		/*A key press is awaited, and then stored in VX */
	 case 0x000A:
	    cpu.V[X].WORD = get_key_pressed();
	    break;
	    
	  break;
	  
		/*Set the delay timer to VX*/
	case 0x0015:
		cpu.dt = X;
	  break;

		/*Set the sound timer to VX */
	case 0x0018:
	    cpu.st = X; 
	   break;

		/*Add VX to ip*/
	case 0x001E:
	    cpu.ip.WORD += cpu.V[X].WORD; 
	  break;

		/*Set ip to the location of the sprite for the character in VX */
	case 0x0029:
	  cpu.ip.WORD = memory[cpu.V[X].WORD];
	  break;

		/*Store the binary-coded decimal representation of VX*/
	case 0x0033:
	  memory[cpu.ip.WORD] = cpu.V[X].WORD / 100;
	  memory[cpu.ip.WORD + 1] = (cpu.V[X].WORD / 10) % 10;
	  memory[cpu.ip.WORD + 2] = cpu.V[X].WORD % 10;
	  break;

		/*Store V0 to VX (including VX) in memory starting at address ip*/
	case 0x0055:
	  reg_dump(X);
	  break;


		/*Fill V0 to VX (including VX) with values from memory starting at address */
	case 0x0065:
	  reg_load(X);
	  break;
	}
      break;
   }
}




 
   

