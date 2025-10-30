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
        OnePassAlgo(vector<vector<string>> inAsm, FileHandler fh) : asmCode(inAsm), memo(vector<int>(216, -1)), fileHandler(fh){

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

                    wordsOccupied.push_back(jmp);
                    memoPos += jmp;
                    continue;
                }

                if(operation.code == "CONST"){

                    if((line.size() - whereIAmInLine) != operation.parameter_spaces)
                        errHandler.logSyntaxError("This number of parameters is not supported", (int)(wordsOccupied.size())+1);

                    int jmp = 1;

                    memo[memoPos] = stoi(line[whereIAmInLine+1]);

                    wordsOccupied.push_back(jmp);
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
        unordered_map<string, tuple<int, bool, vector<int>>> symbolList;
        vector<int> memo;
        vector<int> wordsOccupied;
        FileHandler fileHandler;
        int memoPos = 0;

        void o1(){
            vector<string> lines;

            int memoPos = 0, id = 0;
            vector<int> wordsCopy = wordsOccupied;

            while(id < wordsCopy.size()){

                string line = "end ";

                line += (to_string(memoPos)) + ": ";

                while(wordsCopy[id]){

                    line += (to_string(memo[memoPos])) + " ";
                    memoPos++;
                    wordsCopy[id]--;
                }
                line.push_back('\n');
                lines.push_back(line);
                id++;
            }
            
            lines.push_back("\n\n");

            auto smbList = convertSymbolList();

            for(auto line : smbList)
                lines.push_back(line);
            
            fileHandler.writeFile(".o1", lines);
        }

        void o2(){

            for(auto[ini, ti] : symbolList){

                auto[val, seen, links] = ti;

                while(links.back() != -1){

                    memo[links.back()] = val;
                    links.pop_back();
                }
            }

            vector<string> lines;

            int memoPos = 0, id = 0;

            while(id < wordsOccupied.size()){

                string line = "end ";

                line += (to_string(memoPos)) + ": ";

                while(wordsOccupied[id]){

                    line += (to_string(memo[memoPos])) + " ";
                    memoPos++;
                    wordsOccupied[id]--;
                }
                line.push_back('\n');
                lines.push_back(line);
                id++;
            }
            
            lines.push_back("\n\n");

            auto smbList = convertSymbolList();

            for(auto line : smbList)
                lines.push_back(line);
            
            fileHandler.writeFile(".o2", lines);
        }

        vector<string> convertSymbolList(){
            
            vector<string> tbl;

            tbl.push_back("Simbolo  |  valor  |  definido  |  lista de pendencias  \n");

            for(auto[simbolo, ti] : symbolList){

                auto[val, seen, links] = ti;

                string line = simbolo + "  |  " + to_string(val)  + 
                "  |  " + to_string(seen) + "  |  [";

                string lista = "";

                int id = links.size()-1;
                while(id > 0){
                    lista += to_string(links[id]) + ", ";
                    id--;
                }

                line += lista + to_string(links[0]) + "]  \n";
            }

            return tbl;
        }
};