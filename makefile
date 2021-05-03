options = -Wall -Wextra -pedantic -O3

run: MathBeauty.out
	./MathBeauty.out

MathBeauty.out: MathBeauty.cpp MathBeauty.hpp
	g++ MathBeauty.cpp -mavx2 -lSDL2 $(options) -o MathBeauty.out 
