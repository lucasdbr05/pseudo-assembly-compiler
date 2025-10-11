all: compiler

compiler: main.cpp
	rm -f compiler
	g++ main.cpp -o compiler