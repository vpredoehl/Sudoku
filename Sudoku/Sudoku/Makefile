CFLAGS= -O3 -std=c++11 -stdlib=libc++

all:	Sudoku
clean:
	rm -rf Sudoku Sudoku.dSYM

Sudoku:	main.cpp Sudoku.h
	clang++  $(CFLAGS) main.cpp -o Sudoku

