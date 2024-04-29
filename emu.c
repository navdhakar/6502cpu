#include <stdio.h>
#include <stdlib.h>
#include "MCS6502.h"

#define MEMORY_SIZE (64 * 1024)
typedef unsigned char Byte;
typedef unsigned short Word;

typedef struct{
    Byte Data[MEMORY_SIZE];
}Mem;

// Function to read memory at a specific address
Byte mem_read(Word address, void *readWriteContext) {
    Mem *memory = (Mem *)readWriteContext;
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

int main (){
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
	mem_write(0xFFFC+1, 0x02, memory); 
	Byte rd = mem_read(0xFFFC, memory);
	printf("read from address 0xFFFC data 0x%02X\n", rd); 
    printf("Memory check finished\n");
    printf("64 KB memory initialized\n");
	write_code(memory);
    MCS6502ExecutionContext context;
    MCS6502Init(&context, mem_read, mem_write, memory);
    printf("Reset CPU.\n");
    MCS6502Reset(&context);
    printf("Started 6502 CPU emulator!\n");
	MCS6502Tick(&context);
    free(memory);
    return 0;
}

