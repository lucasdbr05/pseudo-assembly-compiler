#pragma once
#include <bits/stdc++.h>
#include "Symbols.hpp"
#include "FileHandler.hpp"

using namespace std;


struct Macro {
    string name;
    vector<vector<string>> definition;
    int argumentsQtt;

    Macro() = default;

    Macro(string _name, vector<vector<string>> _definition, int _arguments) 
        : name(_name), definition(_definition), argumentsQtt(_arguments) {}
};;

class PreProcessor {
public:
    PreProcessor(FileHandler fh) : fileHandler(fh) {}
    
    FileHandler preProcessAsmCode() {
        vector<string> preProcessedCode;
        vector<string> asmCodeLines = getAsmCode();

        vector<vector<string>> normalizedContent = getNormalizedLineContent(asmCodeLines);

        vector<string> structuredContent = structureContent(normalizedContent);
        
        string finalContent =  joinFinalContent(structuredContent);

        fileHandler.writeFile(".pre", finalContent);

        return FileHandler(fileHandler.getFilenameWithoutExtension() + ".pre");
    }


private:
    
    FileHandler fileHandler;

    map<string, Macro> macroNameDefinitionTable; 

    vector<string> getAsmCode() {
        return fileHandler.readFileLines();
    }

    vector<vector<string>> getNormalizedLineContent(vector<string> asmLines){
        vector<vector<string>> strucuturedContent;

        for(auto& line: asmLines) {
            if(line.size() == 0) continue;

            string lineWithoutComments = stripComments(line);
            vector<string> splitedLine = splitLine(lineWithoutComments);
            splitedLine = normalizedDefaultTokens(splitedLine);
            
            if(splitedLine.size() == 0) continue;

            strucuturedContent.push_back(splitedLine);
        }
        return strucuturedContent;
    }

    vector<string> structureContent(vector<vector<string>>& normalizedContent){
        vector<string> structuredContent;
        string content;
        int contentSize = normalizedContent.size();
        for(int i=0; i< contentSize; i++) {
            vector<string> line = normalizedContent[i];
            int initLoopIndex = 0;

            if(line[0].back() == ':') {
                bool isMacroDefinition = line.size() > 1 && toUppercase(line[1]) == "MACRO";
                if(isMacroDefinition) {
                    i = storeMacroDefinition(normalizedContent, i);
                    continue;
                }
                
                content += line[0] + " ";
                initLoopIndex = 1;
                if(line.size() == 1) continue;
            }
            
            vector<vector<string>> lines = {line};
            
            bool isUsingMacro = macroNameDefinitionTable.count(line[0]);
            if(isUsingMacro) {
                vector<string> arguments;
                for(int i=1; i< line.size(); i++) {
                    bool isArgument = isAlphaNumeric(line[i][0]);
                    if(isArgument) arguments.push_back(line[i]);
                } 
                lines = macroExpantion(line[0], arguments);
            }

            for(auto& line: lines) {   
                for(int i=initLoopIndex; i<line.size(); i++) {
                    if(line[i].size() ==1 && isSimbol(line[i][0])) {
                        content[content.size() - 1] = line[i][0];
                        content += (content.back() == ',' ? " ": "");
                    } else {
                        content += line[i] + " ";
                    }
                }
                structuredContent.push_back(content);
                content= "";
            }
        }

        if (content.size() > 0) {
            structuredContent.push_back(content);
            content= "";
        }

        return structuredContent;
    }

    vector<vector<string>> macroExpantion(
        string macroName, 
        vector<string> arguments
    ) {
        Macro macro = macroNameDefinitionTable[macroName];
        map<string, string> macroArguments;

        vector<vector<string>> newCode = macro.definition;
        vector<vector<string>> result;

        for(int i=0; i<arguments.size(); i++) {
            string arg = "#"+to_string(i+1);
            macroArguments[arg] = arguments[i];
        }

        for(int i=0; i<newCode.size(); i++) {
            bool isUsingMacro = macroNameDefinitionTable.count(newCode[i][0]);
            vector<vector<string>> lines = {newCode[i]};
            if(isUsingMacro) {
                vector<string> arguments;
                for(int j=1; j< newCode[i].size(); j++) {
                    bool isArgument = isAlphaNumeric(newCode[i][j][0]);
                    if(isArgument) arguments.push_back(newCode[i][j]);
                } 
                lines = macroExpantion(newCode[i][0], arguments);
            }

            for(auto& line:lines) {
                for(auto& token: line) {
                    if(macroArguments.count(token)) {
                        token = macroArguments[token];
                    }
                }
                result.push_back(line);
            }
        }

        return result;
    }

    string stripComments(const string& line, char commentChar = ';') {
        size_t pos = line.find(commentChar);
        string out = (pos == string::npos) ? line : line.substr(0, pos);
        while (!out.empty() && isspace(out.back())) {
            out.pop_back();
        }
        return out;
    }

    vector<string> splitLine(const string& line) {
        vector<string> tokens;
        int l=0, r;
        int string_size = line.size();


        r=l;

        while(r < string_size && !isEmpty(line[r]) && line[r]!= ':') r++;

        if(r==string_size || line[r] != ':') r = l;
        else {
            tokens.push_back(line.substr(l, r-l+1));
            r++;
            l=r;
            while(l<string_size && isEmpty(line[l])) l++;
            r=l;
        }

        while(r<string_size) {
            while(r < string_size && isAlphaNumeric(line[r])) r++;

            if(r>l){
                tokens.push_back(line.substr(l, r-l));
            }
            if(isSimbol(line[r])) {
                tokens.push_back(line.substr(r, 1));
                r++;
            }
            l = r;

            while(l<string_size && isEmpty(line[l])) l++;
            r = l;
        }

        return tokens;
    }

    vector<string> normalizedDefaultTokens(vector<string> tokens) {
        for(int i=0; i<tokens.size(); i++) {
            string uppercasedToken = toUppercase(tokens[i]);
            if(defaultNames.count(uppercasedToken)) {
                tokens[i] = uppercasedToken;
            }
        }
        return tokens;
    }

    int storeMacroDefinition(vector<vector<string>>& codeLines, int currentIndex) {
        map<string, string> correspondentArguments;
        string macroName = codeLines[currentIndex][0]; macroName.pop_back();
        vector<vector<string>> macroDefinition;
        for(int i=2; i<codeLines[currentIndex].size(); i++){
            string argument = codeLines[currentIndex][i];
            correspondentArguments[argument] = "#" + to_string((i-1));
        }
        currentIndex++;
        while(
            currentIndex < codeLines.size() && 
            codeLines[currentIndex].size() != 0 && 
            codeLines[currentIndex][0] != "ENDMACRO"
        ) {
            for(int i=0; i<codeLines[currentIndex].size(); i++) {
                string& currentToken = codeLines[currentIndex][i];
                if(correspondentArguments.count(currentToken)) {
                    currentToken = correspondentArguments[currentToken];
                }
            }
            macroDefinition.push_back(codeLines[currentIndex]);
            currentIndex++;
        }

        macroNameDefinitionTable[macroName] = Macro(macroName, macroDefinition, correspondentArguments.size());
        return currentIndex;
    }


    string joinFinalContent(vector<string> content) {
        string result = "";

        for(auto& line: content) result += line + "\n";
        return result;
    }


    bool isAlphaNumeric(char c, bool isLabel = false) {
        return ('0' <= c && c <= '9') || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (isLabel && c == ':');
    }

    bool isEmpty(char c) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }

    bool isSimbol(char c) {
        return c == ',' || c == '+' || c == '-';
    }

    string toUppercase(string str) {
        for (char& c : str) c = toupper(c);
        return str;
    }
};