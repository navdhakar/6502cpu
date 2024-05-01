create an assembly program using 6502 Instruction set for the cpu.

segmenting wozmon not working properly.

so I had to manually set start location for wozmon at 0xFF00, 

compile wozmon
```bash
make wozmon.rom
```

compiling the program
```bash
make basic.rom
``` 
memory segemetations is defined in 6502cpu.cfg
our memory starts at 0x0200
