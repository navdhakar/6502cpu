this is 6502 cpu emulator,
base emulator was picked from here: https://github.com/bzotto/MCS6502
this repo contains memory implementation for this cpu
also wrote little code that can be executed on the cpu

6502 CPU specs:

- registers: a, x, y, sp, pc, p, sp

- memory: 64 KB [(0x0000-0x00FF)-zero page, (0x0100-0x01FF)-stack memory, (0x0200-0xFFF9)-this is where we store code and data, (0xFFFA-0xFFFF)-various, 

- cycle: we can control the cycle rate.

- instuction-set: https://www.masswerk.at/6502/6502_instruction_set.html
compiling:
```bash
gcc/ -g emu.c MCS6502.c -o emu -lm 
```

- ### TODO
- looking to create a way to write code easily into memory
- write some interesting program
