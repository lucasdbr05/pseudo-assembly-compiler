#pragma once
#include <iostream>
#include <string>
#include "PreProcessor.hpp"
#include "Compiler.hpp"
#define endl '\n'

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "To run the compiler use: ./compiler <filename>" << endl;
        return 1;
    }

    string filename = argv[1];


    FileHandler fileHandler = FileHandler(filename);
    PreProcessor preProcessor = PreProcessor(fileHandler);


    FileHandler preProcessFileHandler = preProcessor.preProcessAsmCode();


    Compiller compiler = Compiller(preProcessFileHandler);

    compiler.compile();

    return 0;
}