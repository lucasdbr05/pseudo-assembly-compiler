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

            int currentLine = 1;

            for(auto line : asmCode){

                if(line.empty()) {
                    currentLine++;
                    continue;
                }

                int whereIAmInLine = 0;
                int labelCount = 0;

                for(const auto& token : line) {
                    if(!token.empty() && token.back() == ':') {
                        labelCount++;
                    }
                }
                if(labelCount > 1) {
                    errHandler.logSemanticError("Dois rotulos na mesma linha", currentLine);
                    currentLine++;
                    continue;
                }

                if(!line[whereIAmInLine].empty() && line[whereIAmInLine].back() == ':'){
                    string labelName = line[whereIAmInLine];
                    labelName.pop_back();
                    
                    if(!isValidLabel(labelName)) {
                        errHandler.logLexicalError("Rotulo com formato invalido", currentLine);
                    }

                    auto [pos, seen, links] = symbolList[labelName];

                    if(seen) {
                        errHandler.logSemanticError("Rotulo declarado duas vezes em lugares diferentes", currentLine);
                    }

                    if(!links.size()){
                        symbolList[labelName] = make_tuple(memoPos, true, vector<int>({-1}));
                    }else{
                        symbolList[labelName] = make_tuple(memoPos, true, links);
                    }

                    whereIAmInLine++;
                }

                if(whereIAmInLine >= line.size()) {
                    currentLine++;
                    continue;
                }

                if(!defaultNames.count(line[whereIAmInLine])) {
                    errHandler.logSyntaxError("Instrucao inexistente", currentLine);
                    currentLine++;
                    continue;
                }
                
                Symbol operation;
                bool foundOperation = false;

                for(auto symbol : symbols){
                    if(symbol.code == line[whereIAmInLine]){
                        operation = symbol;
                        foundOperation = true;
                        break; 
                    }
                }
                
                int numParams = line.size() - whereIAmInLine - 1;

                if(operation.code == "SPACE"){
                    if(numParams > operation.parameter_spaces) {
                        errHandler.logSyntaxError("Instrucao com numero de parametros errado", currentLine);
                    }

                    int jmp = 0;
                    if(line.size() == whereIAmInLine+1) jmp++;
                    else jmp += stoi(line[whereIAmInLine+1]);

                    wordsOccupied.push_back(jmp);
                    memoPos += jmp;
                    currentLine++;
                    continue;
                }

                if(operation.code == "CONST"){
                    if(numParams != operation.parameter_spaces) {
                        errHandler.logSyntaxError("Instrucao com numero de parametros errado", currentLine);
                    }

                    int jmp = 1;
                    memo[memoPos] = stoi(line[whereIAmInLine+1]);

                    wordsOccupied.push_back(jmp);
                    memoPos += jmp;
                    currentLine++;
                    continue;
                }
                
                if(numParams != operation.parameter_spaces) {
                    errHandler.logSyntaxError("Instrucao com numero de parametros errado", currentLine);
                }

                memo[memoPos]= operation.opcode;
                memoPos++;
                whereIAmInLine++;
                wordsOccupied.push_back(1+operation.parameter_spaces);
                
                for(int i = whereIAmInLine; i < line.size(); i++){
                    
                    if(!isValidOperand(line[i])) {
                        errHandler.logLexicalError("Operando com formato invalido", currentLine);
                    }

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

                currentLine++;
            }

            for(auto[simbolo, ti] : symbolList){
                auto[val, seen, links] = ti;
                if(!seen && links.size() > 1) {
                    errHandler.logSemanticError("Rotulo nao declarado: " + simbolo, -1);
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

                tbl.push_back(line);
            }

            return tbl;
        }

        bool isValidLabel(const string& label) {
            if(label.empty()) return false;
            
            if(label[0] >= '0' && label[0] <= '9') {
                return false;
            }
            
            for(char c : label) {
                if(!((c >= 'a' && c <= 'z') || 
                     (c >= 'A' && c <= 'Z') || 
                     (c >= '0' && c <= '9') || 
                     c == '_')) {
                    return false;
                }
            }
            
            return true;
        }

        bool isValidOperand(const string& operand) {
            if(operand.empty()) return false;
            
            bool isNumber = true;
            int start = 0;
            if(operand[0] == '-' || operand[0] == '+') start = 1;
            
            for(int i = start; i < operand.size(); i++) {
                if(operand[i] < '0' || operand[i] > '9') {
                    isNumber = false;
                    break;
                }
            }
            
            if(isNumber && operand.size() > start) return true;
            
            return isValidLabel(operand);
        }

};