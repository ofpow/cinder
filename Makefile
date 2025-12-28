run:
	@cc -o main main.c -lm && ./main
o:
	@cc -O3 -o main main.c -lm && ./main
d:
	@cc -O3 -g -o main main.c -lm && gdb -tui ./main
