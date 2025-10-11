#pragma once
#include <iostream>
#include <string>
#include "PreProcessor.hpp"
#define endl '\n'

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "To run the compiler use: ./compiler <filename>" << endl;
        return 1;
    }

    string filename = argv[1];


    FileHandler fileHandler = FileHandler(filename);
    PreProcessor pp = PreProcessor(fileHandler);
    
    pp.preProcessAsmCode();

    return 0;
}