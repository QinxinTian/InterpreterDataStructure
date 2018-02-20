#include "Interpreter.h"

#define stackSize 100


double stack2[stackSize];
int top;
int count;
int n;
string lineFromFile;


void Interpreter::interpretScript(ifstream& inputFile, ofstream& outputFile){



    int lineNumber = 0;

    while (getline(inputFile, lineFromFile))
    {
		replace(lineFromFile.begin(), lineFromFile.end(), '\t', ' ');
        lineNumber++;
        LINE_TYPE lineType = getLineType(lineFromFile); // Check Parser.h for the different line types
        cout << "line " << lineNumber << " " << lineFromFile << " is type: " << lineType << endl;
        // Use your interpreter to execute each line

		
		if (lineType == 1) { // Store the variable name and the corresponding value to the map
			vector<string> splitLine = tokenize(lineFromFile, "=");
			if (splitLine.size() > 2 || splitLine.size() < 1) {
				// not a valid expression
			}
			else if (splitLine.size() == 2) {
				// line with an assignment
				char exp[256]; // Store the expression given in the line
				string name; // Variable name
				char str1[256];
				double result;
				string lhs = splitLine.at(0);
				string rhs = splitLine.at(1);
				
				//Get variable name
				getNextSymbol(lhs);
				name = getNextSymbol(lhs);
				
				//checks if rhs is function 
				int open = rhs.find_first_of('(');
				rhs.erase(remove(rhs.begin(), rhs.end(), ' '), rhs.end());//trim the string
				string temp = rhs.substr(0, open - 1);

				//If the rhs is function
				if (open != -1 && functionMap.find(temp) != functionMap.end()) {
					int open = rhs.find_first_of('(');
					int close = rhs.find_last_of(')');
					string inside = rhs.substr(open + 1, close - open - 1);
					inside.erase(remove(inside.begin(),inside.end(), ' '), inside.end()); //trim the string
					vector<string> parameters = tokenize(inside, ",");
					vector<double> values;
					for (int i = 0; i < parameters.size(); i++) {
						values.push_back(variableMap.at(parameters.at(i)));
					}
					variableMap[name] = functionMap[temp].call(outputFile, variableMap, functionMap, values, false);
					
				}
				else {

					map<string, double> dump;
					strcpy(str1, rhs.c_str());
					postfix2(exp, str1, variableMap, dump);

					// Calculate the expression
					result = calculate(exp);

					// Store the variable to the map
					variableMap[name] = result;
				}
			}
			else {
				// line without an assignment
			}
		}
		



		else if (lineType == 2) {

			vector<string> splitLine = tokenize(lineFromFile, "=");
			//Function call
			if (splitLine.size() == 1) {
				UserFunction newFunc;
				int open = splitLine.at(0).find_first_of('(');
				string temp = splitLine.at(0).substr(0, open);
				newFunc = functionMap.at(temp);
				open = splitLine.at(0).find_first_of('(');
				int close = splitLine.at(0).find_last_of(')');
				string inside = splitLine.at(0).substr(open + 1, close - open - 1);
				vector<string> givenParameter = tokenize(inside, ",");
				
				vector<double> input;
				for (int i = 0; i < givenParameter.size(); i++) {
					input.push_back(variableMap.at(givenParameter.at(i)));
				}
				
				newFunc.call(input, outputFile, functionMap);

				// not a valid expression
			}
			else if (splitLine.size() == 2) {
				// line with an assignment
				char exp[256]; // Store the expression given in the line
				string name; // Variable name
				char str1[256];
				double result;
				string lhs = splitLine.at(0);
				string rhs = splitLine.at(1);

				//Get variable name
				name = getNextSymbol(lhs);

				if (variableMap.find(name) == variableMap.end()) {
 					cout << "Variable " << name << " is not declared." << endl;
					break;
				}
				map<string, double> dump;
				strcpy(str1, rhs.c_str());
				postfix2(exp, str1, variableMap, dump);

				// Calculate the expression
				result = calculate(exp);

				// Store the variable to the map
				variableMap[name] = result;
			}
			else {
				// line without an assignment
			}
		}

		// write the result from the return statement of the program into the output file
		else if (lineType == 3) {
			int startIndex = 0;
			int endIndex = 0;
			//Start Index
			for (int i = 15; i < lineFromFile.size(); i++) {
				if (lineFromFile.at(i) != ' ') {
					startIndex = i;
					break;
				}
			}

			//End Index
			for (int i = lineFromFile.size()-1; i >= 15 ; i--) {
				if (lineFromFile.at(i) != ' ') {
					endIndex = i;
					break;
				}
			}

			//Quotation mark
			if (lineFromFile.at(startIndex) == '\"') {
				string inQuatation = lineFromFile.substr(startIndex + 1, endIndex - startIndex - 1 - 1);
				
					outputFile << lineFromFile.substr(startIndex + 1, endIndex - startIndex - 1 - 1);
				
			}
			//Calling by variable
			else {
				string variable = lineFromFile.substr(startIndex , endIndex - startIndex);
				std::map<std::string, double>::iterator it = variableMap.begin();
				while (it != variableMap.end()) {
					if (variable.compare(it->first) == 0) {
						double temp = it->second;
						if (fmod(temp, 1) == 0) {
							outputFile << temp;
						}
						else {
							string tstring = to_string(temp);
							outputFile << tstring.erase(tstring.find_last_not_of('0') + 1, std::string::npos);
						}
					}
					it++;
				}
			}
			
		}


		//Function definition
		else if (lineType == 4) {
			int open = lineFromFile.find_first_of('(');
			int close = lineFromFile.find_last_of(')');

			//Find and set the number of parameters
			int num_parameter = 0;
			for (int i = 0; i < lineFromFile.size(); i++) {
				if (lineFromFile.at(i) == ',') {
					num_parameter++;
				}
			}
			if (num_parameter == 0) {
				if (close - open == 1) {
					num_parameter = -1;
				}
			}
			num_parameter++;
			UserFunction newFunc;
			newFunc.set_num_parameter(num_parameter);
			

			//Finding the function name
			string name = getNextSymbol(lineFromFile);
			name = getNextSymbol(lineFromFile);
			
			//Assign the parameters
			open = lineFromFile.find_first_of('(');
			close = lineFromFile.find_last_of(')');
			string inside = lineFromFile.substr(open+1, close - open-1);
			inside.erase(remove(inside.begin(), inside.end(), ' '), inside.end()); //trim the string
			vector<string> givenParameter = tokenize(inside, ",");
			newFunc.set_given_parameter(givenParameter);
			

			do {
				getline(inputFile, lineFromFile);
				replace(lineFromFile.begin(), lineFromFile.end(), '\t', ' ');
				lineType = getLineType(lineFromFile);
				if (lineType == 7) {
					int count = 1;
					vector<string> temp = newFunc.get_function_definition();
					temp.push_back(lineFromFile);
					do {
						getline(inputFile, lineFromFile);
						replace(lineFromFile.begin(), lineFromFile.end(), '\t', ' ');
						LINE_TYPE lineType = getLineType(lineFromFile);


						if (lineType == 6) {
							count--;
						}
						if (lineType == 7) {
							count++;
						}
						temp.push_back(lineFromFile);
						newFunc.set_function_definition(temp);
						
					} while (count != 0);
					getline(inputFile, lineFromFile);
					replace(lineFromFile.begin(), lineFromFile.end(), '\t', ' ');

					lineType = getLineType(lineFromFile);
				}
				if (lineType != 6) {
					vector<string> temp = newFunc.get_function_definition();
					temp.push_back(lineFromFile);
					newFunc.set_function_definition(temp);
				}
			} while (lineType != 6); //End of the fuction declaration
			functionMap[name] = newFunc;

		}
		
		
		
		
		
		//If-else statement
		if (lineType == 7) {
			int count = 1;
			vector<string> temp;
			temp.push_back(lineFromFile);
			while (count != 0) {
				getline(inputFile, lineFromFile);
				replace(lineFromFile.begin(), lineFromFile.end(), '\t', ' ');
				LINE_TYPE lineType = getLineType(lineFromFile);
				
				
				if (lineType == 6) {
					count--;
				}
				if (lineType == 7) {
					count++;
				}
				temp.push_back(lineFromFile);
			}
			vector<string> result = solveIfElse(temp, variableMap);
			UserFunction newFunc;
			newFunc.set_function_definition(result);
			newFunc.call(outputFile, variableMap, functionMap, vector<double> {}, true);
		}


		

		


    }

    // write the result from the return statement of the program into the output file
    //outputFile << "output of the document.write lines";
	
}

vector<string> Interpreter::solveIfElse(vector<string> v, map<string,double> input) {

	//If - else if - else TO vector 
	int depth = 0;
	int inside = 0;
	bool check = true;
	vector<vector<string>> depthV;
	vector<vector<string>> insideV;
	vector<string> verify;
	vector<string> temp2;
	depthV.push_back(temp2);
	for (int i = 0; i < v.size(); i++) {
		if (v.at(i).find("if") != string::npos) {
			break;
		}
		ifElseResult.push_back(v.at(i));
		v.erase(v.begin()+i);
		i--;
	}
	int tCount = 0;
	for (int i = v.size() - 1; i >= 0; i--) {
		string temp = v.at(i);
		temp.erase(remove(temp.begin(), temp.end(), ' '), temp.end()); // Trim the string
		if (i - 1 >= 0 && (v.at(i - 1).find("if") != string::npos || v.at(i - 1).find("else") != string::npos)) {
			break;
		}
		if (tCount == 1 && temp != "}") {
			ifElseResultBack.push_back(v.at(i));
		}
		if (temp == "}") {
			tCount++;
		}
		if (tCount >= 2) {
			break;
		}

	}
	depthV.at(0).push_back(v.at(0));
	for (int i = 1; i < v.size(); i++) {
		LINE_TYPE lineType = getLineType(v.at(i));
		if (lineType == 6) {
			depth--;
		}
		if (((depth == 0) && (lineType == 9 || lineType == 8)) || (depth == -1 && lineType == 6)) {
			
				check = false;
			
		}
		
		if (lineType == 7) {
			depth++;
		}
		if (depthV.size() <= depth) {
			vector<string> temp2;
			depthV.push_back(temp2);
		}
		if (depth >= 0) {
			depthV.at(depth).push_back(v.at(i));
		}
		
			if (check) {
				temp2.push_back(v.at(i));
			}
			else {
				bool found = false;
				for (int j = 0; j < temp2.size(); j++) {
					if (temp2.at(j).find("if") != string::npos || temp2.at(j).find("else") != string::npos) {
						found = true;
					}
				}
				if (found) {
					temp2.push_back("}");
				}
				insideV.push_back(temp2);
				//insideV[inside] =temp2;
				temp2.clear();
				check = true;
				/*if (lineType == 8) {
				inside = 2;
				}
				else if (lineType == 9) {
				inside = 1;
				}*/
			}
		
	}

	//extract condition expession
	for (int i = 0; i < depthV.at(0).size(); i++) {
		if (depthV.at(0).at(i).find("else if") != string::npos) {
			verify.push_back(depthV.at(0).at(i));
		}
		else if (depthV.at(0).at(i).find("else") != string::npos) {
			verify.push_back(depthV.at(0).at(i));
		}
		else if (depthV.at(0).at(i).find("if") != string::npos) {
				vector<string>::iterator it = verify.begin();
				verify.insert(it, depthV.at(0).at(i));

		}
	}

	for (int i = 0; i < verify.size(); i++) {
		if (ifVerify(verify.at(i),input)) {
			bool containsIf = false;
			for (int j = 0; j < insideV.at(0).size(); j++) {
				if (insideV.at(i).at(j).find("if") != string::npos) {
					containsIf = true;
					break;
				}
			}
			if (containsIf) {
				return solveIfElse(insideV.at(i), input);
			}
			else {
				vector<string> rr;
				for (int j = 0; j < insideV.at(i).size(); j++) {
					ifElseResult.push_back(insideV.at(i).at(j));
				}
				for (int k = 0; k < ifElseResult.size(); k++) {
					rr.push_back(ifElseResult.at(k));
				}
				for (int k = 0; k < ifElseResultBack.size(); k++) {
					rr.push_back(ifElseResultBack.at(k));
				}
				ifElseResult.clear();
				ifElseResultBack.clear();
				

				return rr;
			}
		}
	}
	vector<string> empty;

	return empty;
}



bool Interpreter::ifVerify(string verify, map<string, double> input) {
	int open = verify.find_first_of('(');
	int end = verify.find_last_of(')');
	string temp = verify.substr(open + 1, end - open-1);
	temp.erase(remove(temp.begin(), temp.end(), ' '), temp.end()); // Trim the string
	if (verify.find(">") != string::npos) {
		vector<string> splitLine = tokenize(temp, ">");
		double temp1 = input.at(splitLine.at(0));
		double temp2 = input.at(splitLine.at(1));
		return temp1 > temp2;
	}
	else if (verify.find("<") != string::npos) {
		vector<string> splitLine = tokenize(temp, "<");
		double temp1 = input.at(splitLine.at(0));
		double temp2 = input.at(splitLine.at(1));
		return temp1 < temp2;
	}
	else if (verify.find("==") != string::npos) {
		vector<string> splitLine = tokenize(temp, "==");
		double temp1 = input.at(splitLine.at(0));
		double temp2 = input.at(splitLine.at(1));
		return temp1 == temp2;
	}
	else if (verify.find("else if") == string::npos &&verify.find("else") != string::npos) {
		return true;
	}
	
	return false;

}

double Interpreter::push(double c)
{
	if (top >= stackSize - 1)
	{
		printf("\nStack overflow");
		exit(1);
	}
	stack2[++top] = c;
	return c;
}

double Interpreter::pop(void)
{
	if (top < 0)
	{
		printf("\nStack is empty");
		exit(1);
	}
	return stack2[top--];
}

int Interpreter::isp(int op)
{
	if (op == '(') return 0;
	if (op == '+' || op == '-') return 1;
	if (op == '*' || op == '/') return 2;
	else return 3;
}

double Interpreter::calculate(char *postfix)
{
	double i;
	double j;
	top = -1;
	while (*postfix)
	{
		if (*postfix == '-')
		{
			
			postfix++;
			if (*postfix <='9' && *postfix >= '0') {
				i = 0;
				j = 0;
				int digit = 1;
				while ((*postfix >= '0' && *postfix <= '9')) {
					i = i * 10 + *postfix - '0';
					postfix++;
				}
				if (*postfix == '.') {
					postfix++;

					while ((*postfix >= '0' && *postfix <= '9')) {
						j += (*postfix - '0') * pow(10, -1 * digit++);
						postfix++;
					}
				}
				i += j;
				i *= -1;
				push(i);
				continue;
			}
			postfix--;
			i = pop();
			double temp = pop() - i;
			push(temp);
			postfix++;
		}
		else if ((*postfix >= '0' && *postfix <= '9') || *postfix == '.')
		{
			i = 0;
			j = 0;
			int digit = 1;
			while ((*postfix >= '0' && *postfix <= '9')) {
				i = i * 10 + *postfix - '0';
				postfix++;
			}
			if (*postfix == '.') {
				postfix++;

				while ((*postfix >= '0' && *postfix <= '9')) {
					j += (*postfix - '0') * pow(10, -1 * digit++);
					postfix++;
				}
			}
			i += j;
			push(i);
		}
		else if (*postfix == '+')
		{
			double temp = pop() + pop();
			push(temp);
			postfix++;
		}


		else if (*postfix == '*')
		{
			double temp = pop() * pop();
			push(temp);
			postfix++;
		}

		else if (*postfix == '/')
		{

			i = pop();
			double temp = pop() / i;
			push(temp);
			postfix++;
		}

		else postfix++;
	}
	return pop();
}

void Interpreter::postfix2(char *postfix, char *infix, map<string, double> &map, std::map<string, double> &param)
{
	char c;
	top = -1;

	while (*infix)
	{
		string s(1, *infix);
		if (*infix == '(')
		{
			push(*infix);
			infix++;
		}
		else if (*infix == ')')
		{
			while (stack2[top] != '(')
			{
				*postfix++ = pop();
				*postfix++ = ' ';
			}
			pop();
			infix++;
		}
		else if (*infix == '+' || *infix == '-' || *infix == '*' || *infix == '/')
		{
			while (top >= 0 && isp(stack2[top]) >= isp(*infix))
			{
				*postfix++ = pop();
				*postfix++ = ' ';
			}
			push(*infix);
			infix++;
		}
		else if ((*infix >= '0' && *infix <= '9') || *infix == '.')
		{
			do {
				*postfix++ = *infix++;

			} while ((*infix >= '0' && *infix <= '9') || *infix == '.');
			*postfix++ = ' ';


		}

		else if ((*infix <= 'z' && *infix >= 'a') || (*infix <= 'Z' && *infix >= 'A')) {
			string s;
			while ((*infix <= 'z' && *infix >= 'a') || (*infix <= 'Z' && *infix >= 'A')) {
				s += *infix++;
			}
			std::map<std::string, double>::iterator it = map.begin();
			while (it != map.end())
			{
				if (s.compare(it->first) == 0) {
					double temp = it->second;
					//int add = (int)temp;
					string temp2 = to_string(temp);
					int sizeOfTemp = temp2.size();
					for (int i = 0; i < sizeOfTemp; i++) {
						*postfix++ = temp2.at(i);
					}
					*postfix++ = ' ';
					break;
				}
				it++;
			}
		}

		else {

			infix++;
		}
	}
	while (top >= 0)
	{
		*postfix++ = pop();
		*postfix++ = ' ';
	}
	postfix--;
	*postfix = 0;


}

