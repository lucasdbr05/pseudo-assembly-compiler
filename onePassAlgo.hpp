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
        OnePassAlgo(vector<vector<string>> inAsm) : asmCode(inAsm){

            errHandler = ErrorHandler();
        };

        void runAlgo(){

            for(auto line : asmCode){

                int whereIAmInLine = 0;

                if(line[whereIAmInLine].back() == ':'){
                    line[whereIAmInLine].pop_back();
                    auto [pos, seen, links] = symbolListo2[line[whereIAmInLine]];
                    if(seen)
                        errHandler.logSemanticError("Redefining Label", (int)(wordsOccupied.size())+1);

                    if(!links.size()){
                        symbolList[line[whereIamInLine]] = make_tuple(memoPos, true, vector<int>({-1}));
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

                memo[memoPos]= operation.opcode;
                memoPos++;
                wordsOccupied.push_back(1+operation.parameter_spaces);
                
                if(
                    (line.size() - whereIAmInLine) != operation.parameter_spaces &&
                    operation.code != "SPACE"
                ){
                     errHandler.logSyntaxError("This number of parameters is not supported", (int)(wordsOccupied.size())+1);
                }

                if(operation.code == "SPACE" && (line.size() - whereIAmInLine) > operation.parameter_spaces)
                     errHandler.logSyntaxError("This number of parameters is not supported", (int)(wordsOccupied.size())+1);
                
                if(operation.code == "SPACE" && operation.code == "CONST"){

                    if (!(line.size() - whereIAmInLine)) memo[memoPos] = 0;
                    else memo[memoPos]= stoi(line[whereIAmInLine]);

                    memoPos++;
                    continue;
                }

                for(int i = whereIAmInLine; i < line.size(); i++){

                    if(symbolList.count(line[i])){

                        auto[pos, seen, links] = symbolList[line[i]];
                        links.push_back(memPos);
                        symbolList[line[i]] = make_tuple(pos, seen, links);
                        memPos++;
                        continue;
                    }

                    symbolList[line[i]] = make_tuple(-1, false, vector<int>({-1, memPos}));
                    memPos++;
                }
            }
            o1();
            o2();
        }
        
    private:
        vector<vector<string>> asmCode;
        ErrorHandler errHandler;
        map<string, tuple<int, bool, vector<int>>> symbolList;
        vector<int> memo(216);
        vector<int> wordsOccupied;
        int memoPos = 0;

        void o1(){
        }

        void o2(){
        }
};