#ifndef CSE250_FALL2015_USERFUNCTION_H
#define CSE250_FALL2015_USERFUNCTION_H
#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <sstream>
#include <vector>
#include <iterator>
#include <math.h>
#include <string>


using namespace std;



// This class is not required, but might be helpful in writing your interpreter
class UserFunction {

private:
    vector<string> functionDefinition;
	vector<string> givenParameter;
	int num_parameter;

public:
	void set_num_parameter(int i);
	void set_given_parameter(vector<string> v);
	vector<string> get_function_definition();
	void set_function_definition(vector<string> v);
	double call(vector<double> parameters, ofstream& outputFile, map<string, UserFunction> functionMap);
	double call(ofstream& outputFile, map<string, double> variableMap, map<string, UserFunction> functionMap, vector<double> parameters, bool print);

};


#endif //CSE250_FALL2015_USERFUNCTION_H
