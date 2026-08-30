bin:

	gcc sb.c -o smallbasic

install:

	gcc sb.c -o /usr/bin/smallbasic
	
clean:
	rm -f smallbasic program.elf output.s output.o
