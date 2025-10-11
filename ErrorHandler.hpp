#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

enum ErrorType {
    LEXICAL,
    SYNTAX,
    SEMANTIC
};

struct Error {
    ErrorType type;
    string message;
    int line;

    Error(ErrorType _type, string _message, int _line = 0)
        : type(_type), message(_message), line(_line) {}
};

class ErrorHandler {
public:
    void logLexicalError(string message, int line = 0) {
        Error error(LEXICAL, message, line);
        errors.push_back(error);
        printError(error);
    }

    void logSyntaxError(string message, int line = 0) {
        Error error(SYNTAX, message, line);
        errors.push_back(error);
        printError(error);
    }

    void logSemanticError(string message, int line = 0) {
        Error error(SEMANTIC, message, line);
        errors.push_back(error);
        printError(error);
    }

    bool hasErrors() const {
        return !errors.empty();
    }

private:
    vector<Error> errors;

    void printError(const Error& error) {
        string typeStr;
        switch (error.type) {
            case LEXICAL: typeStr = "LEXICAL"; break;
            case SYNTAX: typeStr = "SYNTAX"; break;
            case SEMANTIC: typeStr = "SEMANTIC"; break;
        }
        cerr << "[" << typeStr << " " << "ERROR "<< "] ";
        if (error.line != 0) {
            cerr << "Linha " << error.line << ": ";
        }
        cerr << error.message << endl;
    }
};
