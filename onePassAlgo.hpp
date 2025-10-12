#pragma once
#include<iostream>
#include<vector>
#include<string>
#include<map>
#include "FileHandler.hpp"
#include "ErrorHandler.hpp"
#include "Symbols.hpp"

using namespace std;

class OnePassAlgo {

    public:
        OnePassAlgo(vector<vector<string>> inAsm) : asmCode(inAsm){};

        void o1(){
            //rotulo: instr args 

            for(auto line : asmCode){

                
            }
        }

        FileHandler doO2(const FileHandler & fhO1){

        }
    private:
        vector<vector<string>> asmCode;
        map<string, tuple<int, bool, vector<int>>> symbolList;
        vector<pair<int, short>> memo;
        int memoPos = 0;
};