/*I N C L U D E S *****************************************************************************************************************/

#include <stdio.h>
#include <chip8.h>
#include <stdbool.h>
#include <string.h>

/*F U N C T I O N S ****************************************************************************************************************/


bool load_ROM(const char* path)
{
	FILE *f = fopen(path, "rb");
	
	if (!f)
	{
		fprintf(stderr, "Error! File is not opened!");
		return false;
	}
	
	uint16_t offset = 0x0200;
	
	while (!feof(f))
	{
		fread(&memory[offset], sizeof(byte), 1, f);
		++offset;
	}
	
	fclose(f);
	return true;
}


void print_help()
{
	printf("Usage: -s [var] -d [var] [filepath]\n");
	printf("-s - scale\n");
	printf ("-d - frame count\n");
	printf("filepath - input file\n");
}


char* parse_input(int argc, char **argv)
{
	char *f_path = NULL;
	int count;
	
	for (count = 1; count < argc; ++count)
	{
		if ((argv[count][0] == '-') && (strlen(argv[count]) != 2))
		{
			print_help();
			exit(-1);
		}
		
		else if ((argv[count][0] == '-') && (strlen(argv[count]) == 2))
		{
			switch(argv[count][1])
			{
				case 's':
					count++;
					if (count < argc)
						scale = atoi(argv[count]);
					break;
				
				case 'd':
					count++;
					if (count < argc)
						delay = atoi(argv[count]);
					break;
					
					case 't':
					//count++;
					if (count < argc)
						cpu.state = CPU_DEBUG;
					break;
					
				default:
					printf("Undefined parament\n");
					break;
			}
		}
		
		else
		{
			f_path = argv[count];
		}
	}
	
	return f_path;
	
}



int main(int argc, char **argv)
{
	scale = SCALE;
	cpu.state = CPU_RUN;
	
	char *file_path = parse_input(argc, argv);
	
	if (scale == 0)
		scale = SCALE;
		
	if (delay < 0)
		delay = 1;
		
	initialize();
     
	if (!load_ROM(file_path))
	{
		fprintf(stderr, "Error! Coundn't load file!\n");
		SDL_Quit();
		exit(-1);
	}
	
	
	
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "Error! Can't init SDL!\n");
		SDL_Quit();
		exit(-1);
	}
	
	if (!display_init())
	{
		fprintf(stderr, "Error! Can't init the display!\n");
		exit(-1);
	}
	
	
	if (!timer_init())
	{
		fprintf(stderr, "Error! Can't init timer\n");
		exit(-1);
	}
	
	execute();
	 
	
	display_destroy();
	SDL_Quit();
	return 0;
	
}

/*E N D  O F  F I L E **************************************************************************************************************/
