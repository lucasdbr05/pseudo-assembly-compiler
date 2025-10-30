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
        vector<string> code = getPreProcessCode();
        vector<vector<string>> tokensByLine = getSplittedCode(code);

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

    vector<vector<string>> getSplittedCode(vector<string> code) {
        vector<vector<string>> splittedCode;
        for(auto& line: code) {
            auto spllitedLine = splitTokens(line);
            splittedCode.push_back(spllitedLine);
        }  
        return splittedCode;
    }

    vector<string> splitTokens(const string& line) {
        vector<string> tokens;
        int l=0, r;
        int string_size = line.size();
        r=l;

        while(r<string_size) {
            while(r < string_size && !isEmpty(line[r]) && !isSplitterOrOperator(line[r])) r++;

            if(r>l){
                tokens.push_back(line.substr(l, r-l));
            }
           
            while(l<string_size && (isEmpty(line[l])|| isSplitterOrOperator(line[l]))) l++;
            r = l;
        }

        return tokens;
    }


};