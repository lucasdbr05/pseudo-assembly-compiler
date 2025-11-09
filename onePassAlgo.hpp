#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "FileHandler.hpp"
#include "ErrorHandler.hpp"
#include "Symbols.hpp"
#include "Utils.hpp"

using namespace std;

class OnePassAlgo {

    public:
        OnePassAlgo(vector<vector<string>> inAsm, FileHandler fh) : asmCode(inAsm),
        memo(vector<int>(216, -1)),
        offset(vector<int>(216, 0)),
        fileHandler(fh)
        {

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
                        symbolList[line[whereIAmInLine]] = make_tuple(memoPos, true, links);
                    }

                    whereIAmInLine++;
                }

                if(!defaultNames.count(line[whereIAmInLine]))
                    errHandler.logSyntaxError("This instruction doesn't exist", (int)(wordsOccupied.size())+1);
                
                // ele quer q pare a compilação aqui? kkkkkk;
                Symbol operation;

                for(auto symbol : symbols){
                    if(symbol.code == line[whereIAmInLine]){

                        operation = symbol;
                        break; 
                    }
                }
                

                if(
                    (
                    (line.size() - whereIAmInLine-1) != operation.parameter_spaces ||
                    ((line.size() - whereIAmInLine-1) != 2 || !(isANum(line[whereIAmInLine+2])))
                ) &&
                    operation.code != "SPACE"
                ){

                    errHandler.logSyntaxError("This number of parameters is not supported", (int)(wordsOccupied.size())+1);
                }

                if(operation.code == "SPACE"){


                    if((line.size() - whereIAmInLine-1) > operation.parameter_spaces)
                        errHandler.logSyntaxError("This number of parameters is not supported", (int)(wordsOccupied.size())+1);

                    int jmp = 0;

                    if(line.size() == whereIAmInLine+1) jmp++;
                    else jmp += stoi(line[whereIAmInLine+1]);

                    wordsOccupied.push_back(jmp);
                    memoPos += jmp;
                    continue;
                }

                if(operation.code == "CONST"){

                    if((line.size() - whereIAmInLine-1) != operation.parameter_spaces)
                        errHandler.logSyntaxError("This number of parameters is not supported", (int)(wordsOccupied.size())+1);

                    int jmp = 1;

                    memo[memoPos] = stoi(line[whereIAmInLine+1]);

                    wordsOccupied.push_back(jmp);
                    memoPos += jmp;
                    continue;
                }
                
                memo[memoPos] = operation.opcode;
                memoPos++;
                whereIAmInLine++;
                wordsOccupied.push_back(1+operation.parameter_spaces);
                

                for(int i = whereIAmInLine; i < line.size(); i++){
                    if(isANum(line[i])){
                        
                        offset[memoPos-1] += stoi(line[i]);
                        continue;
                    }

                    if(symbolList.count(line[i])){
                        
                        auto[pos, seen, links] = symbolList[line[i]];
                        if(seen) memo[memoPos] = pos;
                        else memo[memoPos] = links.back(), links.push_back(memoPos);
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
        unordered_map<string, tuple<int, bool, vector<int>>> symbolList;
        vector<int> memo;
        vector<int> offset;
        vector<int> wordsOccupied;
        FileHandler fileHandler;
        int memoPos = 0;

        void o1(){
            vector<string> lines;

            int memoPos = 0, id = 0;
            vector<int> wordsCopy = wordsOccupied;
            
            string line = "";
            while(id < wordsCopy.size()){

                while(wordsCopy[id]){

                    line += (to_string(memo[memoPos])) + " ";
                    memoPos++;
                    wordsCopy[id]--;
                }
                // lines.push_back(line);
                id++;
            }
            
            for(auto [symbol, linei] : symbolList){
                auto[val, seen, links] = linei;
                if(links.back() != -1) line += (to_string(val) + " " + to_string(links.back()) + " ");
            }
            
            lines.push_back(line);
            lines.push_back("\n\n");
            fileHandler.writeFile(".o1", lines);
        }

        void o2(){

            for(auto[ini, ti] : symbolList){
                
                auto[val, seen, links] = ti;
                
                while(links.back() != -1){
                    
                    memo[links.back()] = val+offset[links.back()];
                    links.pop_back();
                }
            }
            
            vector<string> lines;
            
            string line = "";
            int memoPos = 0, id = 0;

            while(id < wordsOccupied.size()){
                while(wordsOccupied[id]){

                    line += (to_string(memo[memoPos])) + " ";
                    memoPos++;
                    wordsOccupied[id]--;
                }

                id++;
            }
            
            lines.push_back(line);
            lines.push_back("\n\n");
            
            fileHandler.writeFile(".o2", lines);
        }
};