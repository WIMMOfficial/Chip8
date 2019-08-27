#include <chip8.h>
#include <stdio.h>

void reg_dump(byte index)
{
  unsigned int local_pc = cpu.ip.WORD;
   
  for (int i = 0; i <= index; ++i)
    {
       if (local_pc + 2 >= 4096)
		{
          printf(stderr, "Error! Try to access incorrect address %d\n", local_pc);
		  return;
		}
      memory[local_pc] = cpu.V[i].BYTE.low;
      ++local_pc;
      memory[local_pc] = cpu.V[i].BYTE.high;
      ++local_pc;
    }
  
}


void reg_load(byte index)
{
  unsigned int local_pc = cpu.ip.WORD;
   
  for (int i = 0; i <= index; ++i)
    {
       if (local_pc + 2 >= 4096)
		{
			printf(stderr, "Error! Try to access incorrect address %d\n", local_pc);
			return;
		}
       
      cpu.V[i].BYTE.low = memory[local_pc];
      ++local_pc;
      cpu.V[i].BYTE.high = memory[local_pc];
      ++local_pc;
    }
  
}


byte mem_read(unsigned short pos)
{
	if (pos >= 4096)
	{
		fprintf(stderr, "Error! Incorrect memory address: %d", cpu.pc.WORD);
		exit(-1);
	}
	
	return memory[pos];
}


void mem_write(unsigned short pos, byte value)
{
	if (pos >= 4096)
	{
		fprintf(stderr, "Error! Incorrect memory address: %d", cpu.pc.WORD);
		exit(-1);
	}
	
	memory[pos] = value;
}