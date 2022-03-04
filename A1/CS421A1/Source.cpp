//Kira Siganski 200352511, made for CS421 A1
#include <iostream>
#include <string>
#include <regex>
#include <vector>
using namespace std;

bool stringCheck(const string &input);
bool unifyWithOccursCheck(string term1, string term2);
string unifyWithOccursCheckRecursion(vector<string> term1, vector<string> term2);
string unify(string term1, string term2);
vector<string> substitute(vector<string> term, string sub);
vector<string> convertTovector(string term);

int main() {
	//start input loop. don't want to exit until done testing
	bool run = true;
	string term1;
	string term2;
	//introduction and explaining how this program accepts input
	cout << "This program is used to run the logic unification algorithm unify_with_occurs_check() with input in these forms: \n"
		<< "term1: a term2: X \n"
		<< "term1: X  term2: f(y) \n"
		<< "term1: f(f(X,Y),X)  term2: f(f(V,U),g(U,Y)) \n"
		<< "If you wish to close the program only enter 0 in either term's input. \n\n";

	do {
		cout << "Please enter your input for term 1 \n";
		getline(cin, term1);
		//setting loop end check here too to make closing program faster
		if (term1[0] == '0') {
			run = false;
			break;
		}

		cout << "Please enter your input for term 2 \n";
		getline(cin, term2);
		cout << endl;
		//check if you want to close the program. If so, end the loop and exit
		if (term2[0] == '0') {
			run = false;
			break;
		}

		//here for testing purposes;
		cout << "Term 1:" << term1 << "  Term 2:" << term2 << endl;

		//check input for anything not a-z,A-Z, ',' '(' ')', an empty string or incomplete brackets
		//stringCheck returns true if a nonvalid character is found
		if (!stringCheck(term1) && !stringCheck(term2)) {  //if check passes run unification
			if (unifyWithOccursCheck(term1, term2)) {
				cout << "yes \n\n";
			}
			else {
				cout << "no \n\n";
			}
		}
		
		//clear string just in case
		term1.clear();
		term2.clear();
	} while (run);

	return 0;
}

bool stringCheck(const string &input) {
	regex char_check("[^a-zA-Z,()]");
	regex empty_string_check("[ ]");
	regex valid_term_start_check("[,()]");
	//if character not used in the expressions
	if (regex_search(input, char_check)) {
		cout << "You can only use these characters in an expression: \n"
			<< "a-z for constants \n"
			<< "A-Z for variables \n"
			<< "and ',' '(' ')' to group them \n";
		return true;
	}
	//if empty string
	if (regex_search(input, empty_string_check)) {
		cout << "We cannot unify an empty string \n";
	    return true;
	}

	//regex isn't good for checking if brackets match up so better off just running through the string
	int bracket_count = 0;
	for (int i = 0; i < input.size(); i++) {
		//if (, want to matich with ). Can keep count of ( brackets and lower count if ) is found
		if (input[i] == '(') {
			bracket_count++;
		}
		else if (input[i] == ')') {
			//second check in case we have )(
			if (bracket_count == 0) {
				bracket_count--;
				break;
			}
			bracket_count--;
		}
	}
	if (bracket_count != 0) {
		cout  << "mismatched brackets, cannot unify. \n";
		return true;
	}

	//now to make sure the terms start with a-z or A-Z
	if (regex_match(input, valid_term_start_check)) {
		cout << "term must be a valid variable, constant or expression \n";
		return true;
	}

	return false;
}

bool unifyWithOccursCheck(string term1, string term2) {
	//algortithm based on alg in pg69 of Artificial Inteligence by Goerge F Luger, sixth edition
	//and alg based on Ch2 pgs 20-21 of CS421 notes
	string result;
	vector<string> term1_vector;
	vector<string> term2_vector;	
	
	result = unify(term1, term2);
	if (result == "function") {
		//now we know both terms are different functions and need to be broken up to evaluate, time to convert to vectors to process them
		term1_vector = convertTovector(term1);
		term2_vector = convertTovector(term2);

		//send off to recursive version of this function
		result = unifyWithOccursCheckRecursion(term1_vector, term2_vector);
		if (result == "") {//if empty string returned then comparison failed
			return false;
		}
		
		return true;
	}
	else if (result == "same") {
		cout << "Both terms are the same. \n\n";
		return false;
	}
	else if(result == "") {
		return false;
	}
	else {
		cout << result << endl;
		return true;
	}
}

string unifyWithOccursCheckRecursion(vector<string> term1, vector<string> term2) {
	string replacements = "";

	//if vectors aren't the same size then element mismatch so cannot unify
	if (term1.size() != term2.size()) {
		cout << "Unequal number of elements \n";
		return replacements;
	}

	//temp strings to compare elements
	string element1, element2, result;
	int size = term1.size();
	//iterate through terms & compare
	for (int i = 0; i < size; i++) {
		element1 = term1[i];
		element2 = term2[i];
		
		result = unify(element1, element2);
		//if unify unsuccesful then return false.
		if (result == "") {
			return result;
		}
		else if (result == "function") {//if unify returns both are functions
			vector<string> term1_vector = convertTovector(element1);
			vector<string> term2_vector = convertTovector(element2);
			result = unifyWithOccursCheckRecursion(term1_vector, term2_vector);
			if (result != "same" || result != "") {
				//run the substitution
				term1 = substitute(term1, result);
				term2 = substitute(term2, result);
				cout << result << endl;
			}
		}
		else if(result != "same"){
			//run the substitution
			term1 = substitute(term1, result);
			term2 = substitute(term2, result);
			cout << result << endl;
		}
		
		
	}
	
	return result;
}

string unify(string term1, string term2) {
	string result = "";
	//if both terms are the same
	if (term1 == term2) {
		result = "same";
		return result;
	}
	//If term 1 is a variable
	if (!islower(term1[0])) {
		if (term1.size() == 1) { //only allowing single letter variables for simplicity
			//functions and constants are both denoted with lowercase letters
			if (islower(term2[0])) {
				//if term2 is a constant
				if (term2.size() == 1) {
					result += term1;
					result += " = ";
					result += term2;
					return result;
				}

				//if term2 is a function
				//need to check if term1 is in term2 
				for (int i = 0; i < term2.size(); i++) {
					if (term2[i] == term1[0]) {
						return result;
					}
				}
				//if not in term2 then safe to unify
				result += term1;
				result += " = ";
				result += term2;
				return result;
			}

			//term2 is neither a constant or a function therefore it must be a variable
			result += term1;
			result += " = ";
			result += term2;
			return result;

		}
		else {
			cout << "Variables are only one capitol letter A-Z \n";
			return result;
		}

	}
	//If term 2 is a variable
	if (!islower(term2[0])) {
		if (term2.size() == 1) { //only allowing single letter variables for simplicity
			//functions and constants are both denoted with lowercase letters
			if (islower(term1[0])) {
				//if term2 is a constant
				if (term1.size() == 1) {
					result += term2;
					result += " = ";
					result += term1;
					return result;
				}

				//if term2 is a function
				//need to check if term1 is in term2 
				for (int i = 0; i < term1.size(); i++) {
					if (term1[i] == term2[0]) {
						return result;
					}
				}
				//if not in term2 then safe to unify
				result += term2;
				result += " = ";
				result += term1;
				return result;
			}
			//if we get here, then term1 is neither a function, constant or variable
			return result;
		}
		else {
			cout << "Variables are only one capitol letter A-Z \n";
			return result;
		}
	}
	//if both terms are constants
	if (term1.size() == 1 && term2.size() == 1) {
		//return false if both are different, if the same would've been caught in first check
		//so return null
		return result;
	}
	//if one is a function and the other is a constant
	if (term1.size() == 1 && term2.size() == 1) {
		return result;
	}
	//if both terms are functions
	result = "function";
	return result;
}

vector<string> substitute(vector<string> term, string sub) {
	//want to break up string into part 1, element being replaced
	//and part 2, element replacing 
	string p1 = "", p2 = "";
	int swap = 0;
	for (int i = 0; i < sub.size(); i++) {
		//we built the result string as term + " = " + term
		//so once hit " ", p1 is done. increment i once and mark off our swap check
		if (swap == 1) {
			p2 += sub[i];
		}
		else if (sub[i] == ' ') {
			swap = 1;
			i++;
		}
		else {
			p1 += sub[i];
		}	
	}

	//now that we have our terms it's time to sub them in
	for (int i = 0; i < term.size(); i++) {
		term[i] = regex_replace(term[i], regex(p1), p2);
	}

	return term;
}

vector<string> convertTovector(string term) {
	//converting input string to a vector of strings for each 
	//function is f(X) and we only want the inside eg f(g(Y),X) want to grab g(Y) and X
	vector<string> result;
	string temp = "";
	int size = term.size();
	int bracket_check = 0;
	//iterate through string and seperate out the elements into each part of a linked vector
	//elements are separated by commas but to keep a function as one element need to ensure brackets are closed
	//assuming input is correct, then can skip first two which will be f( and use last with is ) as a break point
	for (int i = 2; i < size; i++) {
		//elements are seperated by commas so search for those
		if (term[i] == ',' && bracket_check == 0) {
			//if in here then found the seperator and outside of a function
			//store temp into a new vector item
			result.push_back(temp);
			//clear temp for new object
			temp.clear();
		}
		else if(term[i] == ')' && bracket_check == 0) {
			/*if we reach a ) and all other brackets are enclose then this is the enclosing bracket 
			for the function we are splitting up*/
			//store temp into a new vector item
			result.push_back(temp);
			//break from the loop	
			break;
		}
		else {
			//use string temp to collect an individual element of the function
			temp += term[i];

			if (term[i] == '(') { //brackets already ensured proper pairs with string check func
				bracket_check++;
			}
			else if (term[i] == ')') {
				bracket_check--;
			}
		}
	}
	return result;
}