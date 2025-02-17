asm: main.o pre_asm.o first_pass.o second_pass.o utils.o symbol_structs.o output_files.o
	gcc -ansi -Wall -pedantic main.o pre_asm.o first_pass.o second_pass.o utils.o symbol_structs.o output_files.o -o asm

main.o: main.c utils.h pre_asm.h first_pass.h second_pass.h output_files.h symbol_structs.h
	gcc -c -ansi -Wall -pedantic main.c

pre_asm.o: pre_asm.c pre_asm.h utils.h globals.h
	gcc -c -ansi -Wall -pedantic pre_asm.c

first_pass.o: first_pass.c first_pass.h utils.h globals.h symbol_structs.h
	gcc -c -ansi -Wall -pedantic first_pass.c

second_pass.o: second_pass.c second_pass.h utils.h globals.h symbol_structs.h
	gcc -c -ansi -Wall -pedantic second_pass.c

utils.o: utils.c utils.h globals.h
	gcc -c -ansi -Wall -pedantic utils.c

symbol_structs.o: symbol_structs.c symbol_structs.h utils.h globals.h
	gcc -c -ansi -Wall -pedantic symbol_structs.c

output_files.o: output_files.c output_files.h utils.h globals.h symbol_structs.h
	gcc -c -ansi -Wall -pedantic output_files.c

clean:
	rm -f asm *.o
