all: compiler

compiler: main.cpp
	g++ main.cpp -o compiler
	
clear:
	rm -f compiler