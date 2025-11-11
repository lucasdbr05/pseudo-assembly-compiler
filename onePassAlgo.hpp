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
        OnePassAlgo(vector<vector<string>> inAsm, FileHandler fh) : 
            asmCode(inAsm),
            memo(vector<int>(216, 0)),
            offset(vector<int>(216, 0)),
            fileHandler(fh)
        {

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
                    errHandler.logSemanticError("Two labels on the same line", currentLine);
                    currentLine++;
                    continue;
                }

                if(!line[whereIAmInLine].empty() && line[whereIAmInLine].back() == ':'){
                    string labelName = line[whereIAmInLine];
                    labelName.pop_back();
                    
                    if(!isValidLabel(labelName)) {
                        errHandler.logLexicalError("Label with invalid format", currentLine);
                    }

                    auto [pos, seen, links] = symbolList[labelName];

                    if(seen) {
                        errHandler.logSemanticError("Label declared twice in different places", currentLine);
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
                    errHandler.logSyntaxError("Instruction does not exist", currentLine);
                    currentLine++;
                    continue;
                }
                if(!defaultNames.count(line[whereIAmInLine]))
                    errHandler.logSyntaxError("This instruction doesn't exist", (int)(wordsOccupied.size())+1);
                
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
                        errHandler.logSyntaxError("Instruction with wrong number of parameters", currentLine);
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
                    string valueStr;
                    int tokensUsed = 1;
                    
                    if(whereIAmInLine+1 < line.size() && (line[whereIAmInLine+1] == "+" || line[whereIAmInLine+1] == "-")) {
                        if(whereIAmInLine+2 < line.size() && isANum(line[whereIAmInLine+2])) {
                            valueStr = line[whereIAmInLine+1] + line[whereIAmInLine+2];
                            tokensUsed = 2;
                        } else {
                            errHandler.logSyntaxError("Instruction with wrong number of parameters", currentLine);
                            currentLine++;
                            continue;
                        }
                    } else if(whereIAmInLine+1 < line.size()) {
                        valueStr = line[whereIAmInLine+1];
                    } else {
                        errHandler.logSyntaxError("Instruction with wrong number of parameters", currentLine);
                        currentLine++;
                        continue;
                    }

                    int jmp = 1;
                    memo[memoPos] = stoi(valueStr);

                    wordsOccupied.push_back(jmp);
                    memoPos += jmp;
                    currentLine++;
                    continue;
                }
                
                if(numParams != operation.parameter_spaces) {
                    errHandler.logSyntaxError("Instruction with wrong number of parameters", currentLine);
                }

                memo[memoPos]= operation.opcode;
                memoPos++;
                whereIAmInLine++;
                wordsOccupied.push_back(1+operation.parameter_spaces);
                
                int qtdArgs = 0;

                for(int i = whereIAmInLine; i < line.size(); i++){
                    
                    // Check if this is a signed number (- or + followed by a number)
                    if((line[i] == "+" || line[i] == "-") && i+1 < line.size() && isANum(line[i+1])) {
                        // This is a signed number, treat it as a constant
                        int value = stoi(line[i] + line[i+1]);
                        memo[memoPos] = value;
                        qtdArgs++;
                        memoPos++;
                        i++; // Skip the next token (the number part)
                        continue;
                    }
                    
                    if(isANum(line[i])){
                        errHandler.logSyntaxError("Isolated number as parameter is not allowed", currentLine);
                        continue;
                    }
                    
                    auto [symbol, arithmeticOffset] = parseOperandWithArithmetic(line[i]);
                    
                    if(!isValidLabel(symbol)) {
                        errHandler.logLexicalError("Operand with invalid format", currentLine);
                    }
                    
                    qtdArgs++;

                    if(symbolList.count(symbol)){
                        auto[pos, seen, links] = symbolList[symbol];
                        if(seen) memo[memoPos] = pos;
                        else memo[memoPos] = links.back(), links.push_back(memoPos);
                        symbolList[symbol] = make_tuple(pos, seen, links);
                    } else {
                        symbolList[symbol] = make_tuple(-1, false, vector<int>({-1, memoPos}));
                    }
                    
                    offset[memoPos] = arithmeticOffset;
                    memoPos++;
                }

                if(qtdArgs != operation.parameter_spaces){

                    errHandler.logSyntaxError("This number of parameters is not supported", (int)(wordsOccupied.size())+1);
                }

                currentLine++;
            }
            for(auto[simbolo, ti] : symbolList){
                auto[val, seen, links] = ti;
                if(!seen && links.size() > 1) {
                    errHandler.logSemanticError("Undeclared label: " + simbolo, -1);
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

        pair<string, int> parseOperandWithArithmetic(const string& operand) {
            string symbol = "";
            int offset = 0;
            
            size_t plusPos = operand.find('+');
            size_t minusPos = operand.find('-');
            
            if(plusPos == string::npos && minusPos == string::npos) {
                return {operand, 0};
            }
            
            size_t opPos;
            bool isPlus;
            
            if(plusPos != string::npos && (minusPos == string::npos || plusPos < minusPos)) {
                opPos = plusPos;
                isPlus = true;
            } else {
                opPos = minusPos;
                isPlus = false;
            }
            
            symbol = operand.substr(0, opPos);
            string numStr = operand.substr(opPos + 1);
            
            if(!numStr.empty() && isANum(numStr)) {
                offset = stoi(numStr);
                if(!isPlus) offset = -offset;
            }
            
            return {symbol, offset};
        }

};