#include <string>
using namespace std;

bool isAlphaNumeric(char c, bool isLabel = false) {
    return ('0' <= c && c <= '9') || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (isLabel && c == ':');
}

bool isEmpty(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool isSplitterOrOperator(char c) {
    return c == ',' || c == '+' || c == '-';
}

string toUppercase(string str) {
    for (char& c : str) c = toupper(c);
    return str;
}