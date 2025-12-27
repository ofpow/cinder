run:
	@cc -o main main.c -lm && ./main > out.ppm
o:
	@cc -O3 -o main main.c -lm && ./main > out.ppm
