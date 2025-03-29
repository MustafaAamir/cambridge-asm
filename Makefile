all:
	gcc main.c -o casm
asm_compile:
	./casm -c
	bash build.sh
	./program
ams_run:
	./casm -v 
