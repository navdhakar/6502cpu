basic.rom:basic.o
	ld65 -o $@ -C 6502cpu.cfg $< 
basic.o:basic.asm
	ca65 $< -o $@ 
wozmon.rom:wozmon.o
	ld65 -o $@ -C 6502cpuwoz.cfg $< 
wozmon.o:wozmon.asm
	ca65 $< -o $@ 
clean:
	rm *.rom *.o
