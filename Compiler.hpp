#pragma once
#include <bits/stdc++.h>
#include "Symbols.hpp"
#include "FileHandler.hpp"
#include "ErrorHandler.hpp"

using namespace std;

class Compiller {
public:
    Compiller(FileHandler fileHandler) : fileHandler(fileHandler) {}

    void compile() {
        o1();
        o2();
    }

    void o1 () {

    }

    void o2() {

    }

private:
    FileHandler fileHandler;

    vector<string> getPreProcessCode() {
        return fileHandler.readFileLines();
    }

    vector<vector<string>> splitTokens() {
        
    }

};