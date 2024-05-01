this is 6502 cpu emulator, that can run assembly programs,

base emulator was picked from here: https://github.com/bzotto/MCS6502

this repo contains memory implementation, programs loading functionality for this cpu.

6502 CPU specs:

- registers: a, x, y, sp, pc, p, sp

- memory: 64 KB [(0x0000-0x00FF)-zero page, (0x0100-0x01FF)-stack memory, (0x0200-0xFFF9)-this is where we store code and data, (0xFFFA-0xFFFF)-various], 

- cycle: we can control the cycle rate.

- instuction-set: https://www.masswerk.at/6502/6502_instruction_set.html

### playing with emulator.
compiling:
```bash
gcc -g emu.c MCS6502.c -o emu -lm 
```

build a program before running it in cpu, available in /programs

running programs:
```bash
./emu programs/basic.rom
```
now we can run wozmon(kind of!!)
```bash
./emu programs/wozmon.rom
```
- our cpu load wozmon
- can run in terminal as dispaly device(need more work)
- need some input device


- ### TODO
- write BIOS
- write some interesting program
wozmon source:

https://github.com/jefftranter/6502

references to study about 6502 architecture and how it works:
- https://www.princeton.edu/~mae412/HANDOUTS/Datasheets/6502.pdf
- https://www.youtube.com/watch?v=lNep0mzGNTU&list=PLjQDRjQfW-85S5QkX8wZbkqichM6TLYYt
- http://www.6502.org/users/obelisk/6502/architecture.html

