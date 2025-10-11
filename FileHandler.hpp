#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

class FileHandler {
public:
    FileHandler(const string& file) : filename(file) {}
    
    string readFile() {
        ifstream file(filename);
        
        if (!file.is_open()) {
            throw runtime_error("Erro ao abrir o arquivo: " + filename);
        }
        
        stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        
        return buffer.str();
    }
    
    vector<string> readFileLines() {
        ifstream file(filename);
        
        if (!file.is_open()) {
            throw runtime_error("Erro ao abrir o arquivo: " + filename);
        }
        
        vector<string> lines;
        string line;
        
        while (getline(file, line)) {
            lines.push_back(line);
        }
        
        file.close();
        return lines;
    }
    
    void writeFile(const string& suffix, const string& content) {
        string outputFilename = removeExtension(filename) + suffix;
        
        ofstream file(outputFilename);
        
        if (!file.is_open()) {
            throw runtime_error("Erro ao criar o arquivo: " + outputFilename);
        }
        
        file << content;
        file.close();
    }
    
    void writeFile(const string& suffix, const vector<string>& lines) {
        string outputFilename = removeExtension(filename) + suffix;
        
        ofstream file(outputFilename);
        
        if (!file.is_open()) {
            throw runtime_error("Erro ao criar o arquivo: " + outputFilename);
        }
        
        for (size_t i = 0; i < lines.size(); i++) {
            file << lines[i];
            if (i < lines.size() - 1) {
                file << "\n";
            }
        }
        
        file.close();
    }
    
    string getFilename() const {
        return filename;
    }

    private:
        string filename;
        
        string removeExtension(const string& name) {
            size_t lastDot = name.find_last_of(".");
            if (lastDot == string::npos) {
                return name;
            }
            return name.substr(0, lastDot);
        }

};
