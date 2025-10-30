#pragma once
#include <string>
using namespace std;

inline bool isAlphaNumeric(char c, bool isLabel = false) {
    return ('0' <= c && c <= '9') || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (isLabel && c == ':');
}

inline bool isEmpty(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

inline bool isSplitterOrOperator(char c) {
    return c == ',' || c == '+' || c == '-';
}

inline string toUppercase(string str) {
    for (char& c : str) c = toupper(c);
    return str;
}