#ifndef UNTITLED_INTERPRETER_H
#define UNTITLED_INTERPRETER_H
#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <string.h>
#include <stack> 
#include "UserFunction.h"
#include "Parser.h"

using namespace std;

class Interpreter {

private:
    // suggestions:
    map<string, double> variableMap; // a map of variable names to Variable values
    map<string, UserFunction> functionMap; // a map of user defined functions
	vector<string> ifElseResult;
	vector<string> ifElseResultBack;
    // stack to remember nesting of if blocks

public:
	vector<string> solveIfElse(vector<string> v, map<string, double> input);
	bool ifVerify(string verify, map<string, double> input);
	void postfix2(char *postfix, char *infix, map<string, double> &map, std::map<string, double> &param);
	double calculate(char *postfix);
	int isp(int op);
	double pop(void);
	double push(double c);

    void interpretScript(ifstream&, ofstream&);

    // suggestions:
    double computeInfix(string); // separate function for computing infix notation
    // Other helper functions

};


#endif //UNTITLED_INTERPRETER_H
