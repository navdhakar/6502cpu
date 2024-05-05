#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include "MCS6502.h"

#define MEMORY_SIZE (64 * 1024)
typedef unsigned char Byte;
typedef unsigned short Word;

#define KBD 0xD010 
#define KBDCR 0xD011
#define DSP 0xD012
#define DSPCR 0xD013
typedef struct{
    Byte Data[MEMORY_SIZE];
}Mem;

// Function to read memory at a specific address
Byte mem_read(Word address, void *readWriteContext) { Mem *memory = (Mem *)readWriteContext;
    return memory->Data[address];
}

// Function to write to memory at a specific address
void mem_write(Word address, Byte value, void *readWriteContext) {
    Mem *memory = (Mem *)readWriteContext;
    memory->Data[address] = value;
}
//writing instuction A9 (LDA) and filling next 8 bit with value 42 , as A9 instuction load immediate byte into accumulator.
void write_code(Mem* memory){
	mem_write(0x0200, 0xA9, memory);
	mem_write(0x0200+1, 0x42, memory);
}
void pma(Word address, Mem* memory){

	Byte rd = mem_read(address, memory);
	printf("data at address 0x%02X -> 0x%02X\n", address, rd); 
}
void load_rom_file(const char *filename, Word startAddress, Mem *memory) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    if (fileSize > MEMORY_SIZE - startAddress) {
        fprintf(stderr, "File %s is too large to fit in memory starting from address 0x%04X\n", filename, startAddress);
        fclose(file);
        exit(1);
    }

    fread(memory->Data + startAddress, fileSize, 1, file);
    fclose(file);
}
void display(Mem* memory){

	static Byte lastValue = 0x00;
	Byte rd = mem_read(0xD012, memory);
	if(rd !=lastValue){
		printf("%c\n", (char)rd-0x80); 
		lastValue = rd;
	}
	
    mem_write(DSP, 0x7F, memory);

}
int kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}
char getch() {
    char buf = 0;
    struct termios old = {0};

    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;

    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");

    return buf;
}
void keyboard_polling(Mem* memory){
    if (kbhit()) // Checking whether any key is pressed or not
    {
        char key = getch(); // get the character from keyboard buffer
        // converting lower case character to upper case because wozmon accept only uppercase
        if(key >= 97 && key <= 122)
            key = key-32;
        // wozmon echo only if values lies in this range
        // lower case =  97  to  122
        // upper case =  65  to  90
        // numbers 0 - 9  =  48  to  57
        // Enter = 13
        // space = 32
        if((key >= 65 && key <= 90) || (key >= 48 && key <= 57) || key == 13 || key == 32){
            // base is 0x80 to convert it to something wozmon echo can understand
            //write key to keyboard
            mem_write(KBD, key+0x80, memory);
            //set bit 7 high => keyboard is ready for reading
            mem_write(KBDCR, 0x80, memory);
			pma(KBD, memory);
			pma(KBDCR, memory);
            //set bit 7 high => keyboard is ready for reading

            //set 'DA' => getting ready for echo from wozmon
            //increment last value by 0x80 because DA should be set
            //as echo routine waits for DA to get cleared
        }
    }
}

int main (int argc, int *argv[]){
	if (argc != 2) {
        printf("Usage: ./emu path/file.rom\n", argv[0]);
        return 1;
    }
	char program_path[100];
 	Mem *memory = (Mem *)malloc(sizeof(Mem));
	if(memory == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

	for(int i =0; i<MEMORY_SIZE;i++){
			memory->Data[i] = 0;
	}
    printf("performing memory check...\n");
	mem_write(0xFFFC, 0x00, memory); 
	mem_write(0xFFFC+1, 0xFF, memory); 
	Byte rd = mem_read(0xFFFC+1, memory);
	printf("read from address 0xFFFC data 0x%02X\n", rd); 
    printf("Memory check finished\n");
    printf("64 KB memory initialized\n");

	//write_code(memory);
	//laoding custom programs
	snprintf(program_path, sizeof(program_path), "%s", argv[1]);
	printf("loading program %s\n", program_path);
	load_rom_file("programs/wozmon.rom", 0xFF00, memory);

	// inititalizing CPU
    MCS6502ExecutionContext context;
    MCS6502Init(&context, mem_read, mem_write, memory);
    printf("Reset CPU.\n");
    MCS6502Reset(&context);
    printf("Started 6502 CPU emulator!\n");
	//here we start executing instructions, still can't figure out progrem way to run it according to clock
	// program counter start from address stored at 0xFFFC
	int cyclesPerSecond = 1000000 / 2; // Assuming a 2 MHz CPU
    int cyclesPerRefresh = cyclesPerSecond / 30;
    int cycleCount = 0;
	  while (1) {
            MCS6502Tick(&context);
            cycleCount++;
			keyboard_polling(memory);
			display(memory);
    }

    free(memory);
    return 0;
}

