#pragma once
#include <bits/stdc++.h>
#include "Operations.hpp"
#include "FileHandler.hpp"

using namespace std;

class PreProcessor {
public:
    PreProcessor(FileHandler fh) : fileHandler(fh) {}
    
    void preProcessAsmCode() {
        vector<string> preProcessedCode;
        vector<string> asmCodeLines = getAsmCode();

        vector<vector<string>> normalizedContent = getNormalizedLineContent(asmCodeLines);

        vector<string> structuredContent = structureNormalizedContent(normalizedContent);
        string finalContent =  joinFinalContent(structuredContent);
        // TODO: expand macros
        fileHandler.writeFile(".pre", finalContent);
    }


private:
    
    FileHandler fileHandler;
    unordered_map<string, Operation> operationsMap = getOperationsMap();

    vector<string> getAsmCode() {
        return fileHandler.readFileLines();
    }

    vector<vector<string>> getNormalizedLineContent(vector<string> asmLines){
        vector<vector<string>> strucuturedContent;

        for(auto& line: asmLines) {
            if(line.size() == 0) continue;

            string lineWithoutComments = stripComments(line);
            vector<string> splitedLine = splitLine(lineWithoutComments);
            splitedLine = normalizedOpcodes(splitedLine);
            
            if(splitedLine.size() == 0) continue;

            strucuturedContent.push_back(splitedLine);
        }
        return strucuturedContent;
    }

    vector<string> structureNormalizedContent(vector<vector<string>>& normalizedContent){
        vector<string> structuredContent;
        string content;

        for(auto& line: normalizedContent) {
            int initLoopIndex = 0;
            if(line[0].back() == ':') {
                content += line[0] + " ";
                initLoopIndex = 1;
                if(line.size() ==1) continue;
            }


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

        if (content.size() > 0) {
            structuredContent.push_back(content);
            content= "";
        }

        return structuredContent;
    }

    vector<string> macroExpander() {
        //TODO
        return vector<string>();
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

    vector<string> normalizedOpcodes(vector<string> tokens) {
        for(int i=0; i<tokens.size(); i++) {
            string uppercasedToken = toUppercase(tokens[i]);
            if(operationsMap.count(uppercasedToken)) {
                tokens[i] = uppercasedToken;
            }
        }
        return tokens;
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