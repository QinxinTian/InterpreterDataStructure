
#include "UserFunction.h"
#include "Interpreter.h"



using namespace std;



void UserFunction::set_num_parameter(int i) {
	num_parameter = i;
}

void UserFunction::set_given_parameter(vector<string> v) {
	givenParameter = v;
}

vector<string> UserFunction::get_function_definition() {
	return functionDefinition;
}

void UserFunction::set_function_definition(vector<string> v) {
	functionDefinition = v;
}





double UserFunction::call(ofstream& outputFile, map<string, double> variableMap, map<string, UserFunction> functionMap, vector<double> parameters, bool print) {
	for (int i = 0; i < givenParameter.size(); i++) {
		variableMap[givenParameter.at(i)] = parameters.at(i);
	}
	for (int i = 0; i < functionDefinition.size(); i++) {
		string lineFromFile = functionDefinition.at(i);
		LINE_TYPE lineType = getLineType(lineFromFile);
		vector<string> splitLine = tokenize(lineFromFile, "=");

		if (lineType == 1) {
			if (splitLine.size() == 1) {
				UserFunction newFunc;
				int open = splitLine.at(0).find_first_of('(');
				string temp = splitLine.at(0).substr(0, open);
				temp.erase(remove(temp.begin(), temp.end(), ' '), temp.end()); // Trim the string
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
				lhs.erase(remove(lhs.begin(), lhs.end(), ' '), lhs.end()); // Trim the string
				name = lhs;

				//checks if rhs is function 
				int open = rhs.find_first_of('(');
				rhs.erase(remove(rhs.begin(), rhs.end(), ' '), rhs.end());//trim the string
				string temp = rhs.substr(0, open - 1);
				open = rhs.find_first_of('(');
				int close = rhs.find_last_of(')');
				rhs.erase(remove(rhs.begin(), rhs.end(), ' '), rhs.end());//trim the string
				string inside = rhs.substr(open + 1, close - open - 1);
				inside.erase(remove(inside.begin(), inside.end(), ' '), inside.end()); //trim the string
				vector<string> parameter = tokenize(inside, ",");
				bool contains = false;
				map<string, double> param;
				for (int i = 0; i < givenParameter.size(); i++) {
					param[givenParameter.at(i)] = parameters.at(i);
				}
				if (open != -1 && functionMap.find(temp) != functionMap.end()) {
					string inside = rhs.substr(open + 1, close - open - 1);
					inside.erase(remove(inside.begin(), inside.end(), ' '), inside.end()); //trim the string
					vector<string> parameters = tokenize(inside, ",");
					vector<double> values;
					for (int i = 0; i < parameters.size(); i++) {
						values.push_back(variableMap.at(parameters.at(i)));
					}
					variableMap[name] = functionMap[temp].call(outputFile, variableMap, functionMap, values, false);

				}
				else {

					strcpy(str1, rhs.c_str());

					Interpreter cal;
					cal.postfix2(exp, str1, param, param);

					// Calculate the expression
					result = cal.calculate(exp);

					// Store the variable to the map
					variableMap[name] = result;
				}
			}
			else {
				// line without an assignment
			}
		}




		else if (lineType == 2) {
			//Function call
			if (splitLine.size() == 1) {
				UserFunction newFunc;
				int open = splitLine.at(0).find_first_of('(');
				string temp = splitLine.at(0).substr(0, open);
				temp.erase(remove(temp.begin(), temp.end(), ' '), temp.end()); // Trim the string
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
			}
			if (splitLine.size() == 2) {
				string lhs = splitLine.at(0);
				//trim
				lhs.erase(0, lhs.find_first_not_of(" \n\r\t"));
				lhs.erase(lhs.find_last_not_of(" \n\r\t") + 1);
				string rhs = splitLine.at(1);
				double result;
				char exp[256]; // Store the expression given in the line
				string name; // Variable name
				char str1[256];

				//Get variable name

				name = lhs;
				//checks if rhs is function 
				rhs.erase(remove(rhs.begin(), rhs.end(), ' '), rhs.end());//trim the string
				int open = rhs.find_first_of('(');
				
				string temp = rhs.substr(0, open);
				int close = rhs.find_last_of(')');
				string inside = rhs.substr(open + 1, close - open - 1);
				inside.erase(remove(inside.begin(), inside.end(), ' '), inside.end()); //trim the string
				vector<string> parameter = tokenize(inside, ",");
				bool contains = false;

				map<string, double> param;
				for (int i = 0; i < givenParameter.size(); i++) {
					param[givenParameter.at(i).substr(0, 1)] = parameters.at(i);
				}
				if (open != -1 && functionMap.find(temp) != functionMap.end()) {
					string inside = rhs.substr(open + 1, close - open - 1);
					inside.erase(remove(inside.begin(), inside.end(), ' '), inside.end()); //trim the string
					vector<string> parameters = tokenize(inside, ",");
					vector<double> values;
					for (int i = 0; i < parameters.size(); i++) {
						values.push_back(variableMap.at(parameters.at(i)));
					}
					variableMap[name] = functionMap[temp].call(outputFile, variableMap, functionMap, values, false);

				}

				else {
					strcpy(str1, rhs.c_str());

					map<string, double> dump;
					Interpreter cal;
					cal.postfix2(exp, str1, variableMap, dump);

					// Calculate the expression
					result = cal.calculate(exp);

					// Store the variable to the map
					variableMap[name] = result;
				}
			}
			else {
				// line without an assignment
			}
		}





		if (lineType == 3) {
			int startIndex = lineFromFile.find_first_of('(');
			int endIndex = lineFromFile.find_last_of(')');


			//Quotation mark
			if (lineFromFile.at(startIndex + 1) == '\"') {
				string inQuatation = lineFromFile.substr(startIndex + 2, endIndex - startIndex - 3);
					outputFile << inQuatation;
			}
			//Calling by variable
			else {
				string variable = lineFromFile.substr(startIndex + 1, endIndex - startIndex - 1);
				std::map<std::string, double>::iterator it = variableMap.begin();
				while (it != variableMap.end()) {
					if (variable.compare(it->first) == 0) {
						double temp = it->second;
						//	int add = (int)temp;
						//	string temp2 = to_string(add);
						//	outputFile << temp2;
						if (fmod(temp, 1) == 0) {
							outputFile << temp;
						}
						else {
							string tstring = to_string(temp);
							if (print) {
								outputFile << tstring.erase(tstring.find_last_not_of('0') + 1, std::string::npos);
							}
						}
					}
					it++;
				}
			}

		}



		else if (lineType == 5) {
			string lhs = splitLine.at(0);
			//Get variable name
			getNextSymbol(lhs);
			lhs.erase(remove(lhs.begin(), lhs.end(), ' '), lhs.end());//trim the string
			string name = lhs;
			return variableMap[name];
		}
		else if (lineType == 7) {
			vector<string> temp;
			int count = 0;
			string input;
			do {
				input = functionDefinition.at(i);
				LINE_TYPE lineType = getLineType(input);


				if (lineType == 6) {
					count--;
				}
				if (lineType == 7) {
					count++;
				}
				temp.push_back(input);
				++i;
			} while (count != 0);
			Interpreter inter;
			vector<string> a = inter.solveIfElse(temp, variableMap); //if Empty not going to work
			bool contain_return = false;
			double final;
			if (a.size() != 0) {

				UserFunction newFunc;
				newFunc.set_function_definition(a);
				final = newFunc.call(outputFile, variableMap, functionMap, vector<double>{}, false);
			}
		}
	}
	return 0;
	}








double UserFunction::call(vector<double> parameters, ofstream& outputFile, map<string, UserFunction> functionMap) {
	map<string, double> localVariable;
	//Iterate through the function definition
	for (int i = 0; i < functionDefinition.size(); i++) {
		string lineFromFile = functionDefinition.at(i);
		LINE_TYPE lineType = getLineType(lineFromFile);
		vector<string> splitLine = tokenize(lineFromFile, "=");
		if (lineType == 1) {
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
				lhs.erase(remove(lhs.begin(), lhs.end(), ' '), lhs.end());//trim the string
				name = lhs;

				//checks if rhs is function 
				int open = rhs.find_first_of('(');
				rhs.erase(remove(rhs.begin(), rhs.end(), ' '), rhs.end());//trim the string
				string temp = rhs.substr(0, open - 1);
				open = rhs.find_first_of('(');
				int close = rhs.find_last_of(')');
				rhs.erase(remove(rhs.begin(), rhs.end(), ' '), rhs.end());//trim the string
				string inside = rhs.substr(open + 1, close - open - 1);
				inside.erase(remove(inside.begin(), inside.end(), ' '), inside.end()); //trim the string
				vector<string> parameter = tokenize(inside, ",");
				bool contains = false;

				if (open != -1) {
				std::map<string, UserFunction>::iterator it = functionMap.begin();
				while (it != functionMap.end()) {
					vector<double> input;
					input.push_back(localVariable[inside]);
					if (temp.compare(it->first) == 0) {
						it->second.call(outputFile, localVariable, functionMap, input, true);
					}
					it++;
				}


				for (int i = 0; i < givenParameter.size(); i++) {
					localVariable[givenParameter.at(i)] = parameters.at(i);
				}
			}
				else {
					strcpy(str1, rhs.c_str());

					map<string, double> dump;
					Interpreter cal;
					cal.postfix2(exp, str1, localVariable, dump);

					// Calculate the expression
					result = cal.calculate(exp);

					// Store the variable to the map
					localVariable[name] = result;
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
				temp.erase(remove(temp.begin(), temp.end(), ' '), temp.end()); // trim the string
				newFunc = functionMap.at(temp);
				open = splitLine.at(0).find_first_of('(');
				int close = splitLine.at(0).find_last_of(')');
				string inside = splitLine.at(0).substr(open + 1, close - open - 1);
				vector<string> givenParameter = tokenize(inside, ",");
				vector<double> input;
				for (int i = 0; i < givenParameter.size(); i++) {
					input.push_back(localVariable.at(givenParameter.at(i)));
				}
				newFunc.call(input, outputFile, functionMap);

			}
			else if (splitLine.size() == 2) {
				string lhs = splitLine.at(0);
				//trim
				lhs.erase(0, lhs.find_first_not_of(" \n\r\t"));
				lhs.erase(lhs.find_last_not_of(" \n\r\t") + 1);
				string rhs = splitLine.at(1);
				double result;
				char exp[256]; // Store the expression given in the line
				string name; // Variable name
				char str1[256];

				//Get variable name
				lhs.erase(remove(lhs.begin(), lhs.end(), ' '), lhs.end()); // Trim the string
				name = lhs;

				//checks if rhs is function 
				int open = rhs.find_first_of('(');
				rhs.erase(remove(rhs.begin(), rhs.end(), ' '), rhs.end());//trim the string
				string temp = rhs.substr(0, open - 1);
				open = rhs.find_first_of('(');
				int close = rhs.find_last_of(')');
				rhs.erase(remove(rhs.begin(), rhs.end(), ' '), rhs.end());//trim the string
				string inside = rhs.substr(open + 1, close - open - 1);
				inside.erase(remove(inside.begin(), inside.end(), ' '), inside.end()); //trim the string
				vector<string> parameter = tokenize(inside, ",");
				bool contains = false;

				/*
				std::map<string, UserFunction>::iterator it = functionMap.begin();
				while (it != functionMap.end()) {
					vector<double> input;
					input.push_back(localVariable[inside]);
					if (temp.compare(it->first) == 0) {
						it->second.call(outputFile, localVariable, input, true);
					}
					it++;
				}*/


				for (int i = 0; i < givenParameter.size(); i++) {
					localVariable[givenParameter.at(i)] = parameters.at(i);
				}


				strcpy(str1, rhs.c_str());

				map<string, double> dump;
				Interpreter cal;
				cal.postfix2(exp, str1, localVariable, dump);

				// Calculate the expression
				result = cal.calculate(exp);

				// Store the variable to the map
				localVariable[name] = result;
			}
			else {
				// line without an assignment
			}
		}




		else if (lineType == 3) {
			int startIndex = lineFromFile.find_first_of('(');
			int endIndex = lineFromFile.find_last_of(')');
			

			//Quotation mark
			if (lineFromFile.at(startIndex+1) == '\"') {
				string inQuatation = lineFromFile.substr(startIndex + 2, endIndex - startIndex - 3);
				
				outputFile << inQuatation;
			}
			//Calling by variable
			else {
				for (int i = 0; i < givenParameter.size(); i++) {
					localVariable[givenParameter.at(i)] = parameters.at(i);
				}
				string variable = lineFromFile.substr(startIndex+1, endIndex - startIndex-1);
				std::map<std::string, double>::iterator it = localVariable.begin();
				while (it != localVariable.end()) {
					if (variable.compare(it->first) == 0) {
						double temp = it->second;
						//	int add = (int)temp;
						//	string temp2 = to_string(add);
						//	outputFile << temp2;
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



		else if (lineType == 5) {
			string lhs = splitLine.at(0);
			//Get variable name
			getNextSymbol(lhs);
			string name = getNextSymbol(lhs);
			return localVariable[name];
		}
		else if (lineType == 7) {
			vector<string> temp;
			int count = 0;
			string input;
			do {
				input = functionDefinition.at(i);
				LINE_TYPE lineType = getLineType(input);


				if (lineType == 6) {
					count--;
				}
				if (lineType == 7) {
					count++;
				}
				temp.push_back(input);
				++i;
			} while (count != 0);
			Interpreter inter;
			vector<string> a = inter.solveIfElse(temp,localVariable); //if Empty not going to work
			bool contain_return = false;
			double final;
			if (a.size() != 0) {
				for (int i = 0; i < a.size(); i++) {
					if (a.at(i).find("return") != string::npos) {
						contain_return = true;
					}
				}
				UserFunction newFunc;
				newFunc.set_function_definition(a);
				final = newFunc.call(outputFile, localVariable, functionMap, vector<double>{}, false);
			}
			if (contain_return) {
				return final;
			}
		}
	}
	return 0;
}



