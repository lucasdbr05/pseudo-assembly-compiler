#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdlib>
using namespace std;

struct Operation {
    string code;
    int opcode;
    int parameter_spaces;

    Operation() = default;

    Operation(string _code, int _opcode, int _parameter_spaces) {
        code = _code;
        opcode = _opcode;
        parameter_spaces = _parameter_spaces;
    }
};

const vector<Operation> operations = {
    Operation("ADD", 1, 1),
    Operation("SUB", 2, 1),
    Operation("MULT", 3, 1),
    Operation("DIV", 4, 1),
    Operation("JMP", 5, 1),
    Operation("JMPN", 6, 1),
    Operation("JMPP", 7, 1),
    Operation("JMPZ", 8, 1),
    Operation("COPY", 9, 2),
    Operation("LOAD", 10, 1),
    Operation("STORE", 11, 1),
    Operation("INPUT", 12, 1),
    Operation("OUTPUT", 13, 1),
    Operation("STOP", 14, 0)
};


const set<string> opcodeNames = {
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


