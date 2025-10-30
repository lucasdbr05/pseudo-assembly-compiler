#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "FileHandler.hpp"
#include "ErrorHandler.hpp"
#include "Symbols.hpp"

using namespace std;

class OnePassAlgo {

    public:
        OnePassAlgo(vector<vector<string>> inAsm) : asmCode(inAsm), memo(vector<int>(216, -1)){

            errHandler = ErrorHandler();
        };

        void runAlgo(){

            for(auto line : asmCode){

                int whereIAmInLine = 0;

                if(line[whereIAmInLine].back() == ':'){
                    line[whereIAmInLine].pop_back();
                    auto [pos, seen, links] = symbolList[line[whereIAmInLine]];
                    if(seen)
                        errHandler.logSemanticError("Redefining Label", (int)(wordsOccupied.size())+1);

                    if(!links.size()){
                        symbolList[line[whereIAmInLine]] = make_tuple(memoPos, true, vector<int>({-1}));
                    }else{
                        symbolList[line[whereIAmInLine]] = make_tuple(pos, true, links);
                    }

                    whereIAmInLine++;
                }

                if(!defaultNames.count(line[whereIAmInLine]))
                    errHandler.logSyntaxError("This directive doesn't exist", (int)(wordsOccupied.size())+1);
                
                // ele quer q pare a compilação aqui? kkkkkk;
                Symbol operation;

                for(auto symbol : symbols){
                    if(symbol.code == line[whereIAmInLine]){

                        operation = symbol;
                        break; 
                    }
                }
                
                if(
                    (line.size() - whereIAmInLine) != operation.parameter_spaces &&
                    operation.code != "SPACE"
                ){
                     errHandler.logSyntaxError("This number of parameters is not supported", (int)(wordsOccupied.size())+1);
                }
                
                if(operation.code == "SPACE"){

                    if((line.size() - whereIAmInLine) > operation.parameter_spaces)
                        errHandler.logSyntaxError("This number of parameters is not supported", (int)(wordsOccupied.size())+1);

                    int jmp = 0;

                    if(line.size() == whereIAmInLine+1) jmp++;
                    else jmp += stoi(line[whereIAmInLine+1]);

                    memoPos += jmp;
                    continue;
                }

                if(operation.code == "CONST"){

                    if((line.size() - whereIAmInLine) != operation.parameter_spaces)
                        errHandler.logSyntaxError("This number of parameters is not supported", (int)(wordsOccupied.size())+1);

                    int jmp = 1;

                    memo[memoPos] = stoi(line[whereIAmInLine+1]);

                    memoPos += jmp;
                    continue;
                }
                
                memo[memoPos]= operation.opcode;
                memoPos++;
                wordsOccupied.push_back(1+operation.parameter_spaces);
                

                for(int i = whereIAmInLine; i < line.size(); i++){

                    if(symbolList.count(line[i])){

                        auto[pos, seen, links] = symbolList[line[i]];
                        if(seen) memo[memoPos] = pos;
                        else links.push_back(memoPos);
                        symbolList[line[i]] = make_tuple(pos, seen, links);
                        memoPos++;
                        continue;
                    }

                    symbolList[line[i]] = make_tuple(-1, false, vector<int>({-1, memoPos}));
                    memoPos++;
                }
            }
            o1();
            o2();
        }
        
    private:
        vector<vector<string>> asmCode;
        ErrorHandler errHandler;
        map<string, tuple<int, bool, vector<int>>> symbolList;
        vector<int> memo;
        vector<int> wordsOccupied;
        int memoPos = 0;

        void o1(){

        }

        void o2(){
        }
};