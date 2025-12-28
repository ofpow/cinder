run:
	@cc -fopenmp -Wall -Wextra -O3 -o main main.c -lm && ./main
d:
	@cc -fopenmp -Wall -Wextra -O3 -g -o main main.c -lm && gdb -tui ./main
