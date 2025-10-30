#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <map>
#include <cstdlib>
using namespace std;

struct Symbol {
    string code;
    int opcode;
    int parameter_spaces;

    Symbol() = default;

    Symbol(string _code, int _opcode, int _parameter_spaces) {
        code = _code;
        opcode = _opcode;
        parameter_spaces = _parameter_spaces;
    }
};

const vector<Symbol> symbols = {
    Symbol("ADD", 1, 1),
    Symbol("SUB", 2, 1),
    Symbol("MULT", 3, 1),
    Symbol("DIV", 4, 1),
    Symbol("JMP", 5, 1),
    Symbol("JMPN", 6, 1),
    Symbol("JMPP", 7, 1),
    Symbol("JMPZ", 8, 1),
    Symbol("COPY", 9, 2),
    Symbol("LOAD", 10, 1),
    Symbol("STORE", 11, 1),
    Symbol("INPUT", 12, 1),
    Symbol("OUTPUT", 13, 1),
    Symbol("STOP", 14, 0)
};


const set<string> defaultNames = {
    "ADD",
    "SUB",
    "MULT",
    "DIV",
    "JMP",
    "JMPN",
    "JMPP",
    "JMPZ",
    "COPY",
    "LOAD",
    "STORE",
    "INPUT",
    "OUTPUT",
    "STOP",
    "MACRO",
    "ENDMACRO",
    "SPACE",
    "CONST"
};


