#include <chip8.h>
#include <stdio.h>

void reg_dump(byte index)
{
  uint16_t local_pc = cpu.ir.WORD;
   
  for (int i = 0; i <= index; ++i)
    {
       if (local_pc >= 4096)
		{
          fprintf(stderr, "Error! Try to access incorrect address %d\n", local_pc);
		  return;
		}
      memory[local_pc] = cpu.V[i];
      ++local_pc;
    }
  
}


void reg_load(byte index)
{
  uint16_t local_pc = cpu.ir.WORD;
   
  for (int i = 0; i <= index; ++i)
    {
       if (local_pc >= 4096)
		{
			fprintf(stderr, "Error! Try to access incorrect address %d\n", local_pc);
			return;
		}
       
      cpu.V[i] = memory[local_pc];
      ++local_pc;
    }
  
}


byte mem_read_byte(uint16_t pos)
{
	if (pos >= 4096)
	{
		fprintf(stderr, "Error! Incorrect memory address: %d", cpu.pc.WORD);
		exit(-1);
	}
	
	return memory[pos];
}


void mem_write_byte(uint16_t pos, byte value)
{
	if (pos >= 4096)
	{
		fprintf(stderr, "Error! Incorrect memory address: %d", cpu.pc.WORD);
		exit(-1);
	}
	
	memory[pos] = value;
}

uint16_t mem_read_word(uint16_t pos)
{
	if (pos  >= 4096)
	{
		fprintf(stderr, "Error! Incorrect memory address: %d", cpu.pc.WORD);
		exit(-1);
	}
	
	word ret_val;
	ret_val.WORD = memory[pos] << 8;
	++pos;
	ret_val.WORD = memory[pos] << 4;
	return ret_val.WORD;
}


void mem_write_word(uint16_t pos, uint16_t value)
{
	if (pos  >= 4096)
	{
		fprintf(stderr, "Error! Incorrect memory address: %d", cpu.pc.WORD);
		exit(-1);
	}
	
	memory[pos] = value >> 8;
	++pos;
	memory[pos] = (value & 0x00FF);
	
}
