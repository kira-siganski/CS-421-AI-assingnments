#include "CSPgen.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <random>
using namespace std;
//global var for entire problem. don't want to keep copying this so passing pointers for it
CSP generated_constraint;

CSP* generateCSP(int vars, double tightness, double alpha, double r, int num_parent, int pareto) {
	//print constraints given
	print_input(vars, tightness, alpha, r, num_parent, pareto);
	//generate CSP
	//step 1 - generate r *nlogn distinct random constraints - need to check if constraint exists when generated before adding otherwise need to regenerate
	int constraint_num = round(r * vars * log(vars));

	//initialize pointer to our CSP container
	CSP* generated_csp = &generated_constraint;
	//clear all just in case
	generated_csp->constraints.clear();
	generated_csp->vars.clear();
	generated_csp->var_domains.clear();
	generated_csp->solutions.clear();

	vector<pairs>* constraints = &generated_csp->constraints;
	int a, b;
	pairs pair;
	//generating in pairs of Xa, Xb where a and b are integers 
	for (int i = 0; i < constraint_num; i++) {
		pair = generatePair(vars);

		//now that we have a pair, need to check if already exists
		while (comparePairs(*constraints, pair)) {
			//regen both could be costly if we keep getting repeats 
			//but also possiblity of getting stuck locally if we only regen one until it works
			//so regen both
			pair = generatePair(vars);
		}
		constraints->push_back(pair);
	}

	//step 2 - iterate through constraint list to generate p*d^2 distinct compatible pairs
	int domain = returnDomain(vars, alpha);
	int tuples_num = round(tightness * pow(domain,2));
	tuples tuple;
	//we want to generate the required amount of tuples for each constraint pair
	for (int i = 0; i < constraint_num; i++) {
		//so in here we loop again for the number of tuples needed
		for (int j = 0; j < tuples_num; j++) {
			//generate tuple
			tuple = generateTuple(domain);
			//make sure tuple doesn't already exist and regen if it does
			while (compareTuples((*constraints)[i].tuple_list, tuple)) {
				tuple = generateTuple(domain);
			}
			//add the unique tuple onto the list
			(*constraints)[i].tuple_list.push_back(tuple);
		}
	}

	//new step 3 - create domain of each var in in the CSP
	generated_constraint.domain = domain;
	createVarDomain(generated_csp, vars);

	//new step 4 - generate parents for each var and initalize var list
	//list to hold all valid parents
	vector<variables>* var_list = &generated_csp->vars;
	vector<int> parent_list;
	for (int i = 0; i < vars; i++) {
		variables current_var;
		current_var.name = i;
		current_var.parent_names = generateParents(num_parent, parent_list);
		var_list->push_back(current_var);
		//after each loop, add var i to list of potential parents
		parent_list.push_back(i);
	}

	//new step 5 - generate CPT tables for each var
	//function handles calculations on how many based on number of parents
	for (int i = 0; i < vars; i++) {
		generateCPTTables(&(*var_list)[i], domain);
	}

	//print CSP, printing vars that were calculate here since passing them would not be fun and easier to mess up
	cout << "Generated RB instance:"
		<< "\n Domain size: " << domain
		<< "\n Number of constraints:" << constraint_num
		<< "\n Number of incompatible tuples:" << tuples_num;
	cout << "\n Variables: {"; 
	for (int i = 0; i < vars; i++) {
		cout << "X" << i;
		if (i == (vars - 1)) {
			cout << "}";
		}
		else {
			cout << ", ";
		}
	}
	cout << "\n Domain: {";
	for (int i = 0; i < domain; i++) {
		cout << i;
		if (i == (domain - 1)) {
			cout << "}";
		}
		else {
			cout << ", ";
		}
	}
	//this is what prints out the constraint pairs and their tuples
	print_CSP(*constraints);
	printCPT(var_list, domain);

	return generated_csp;
}

void print_input(int vars, double tightness, double alpha, double r, int np, int pareto) {
	cout << "Input Parameters:"
		<< "\n Number of variables (n): " << vars
		<< "\n Constraint tightness (p): " << tightness
		<< "\n Constant alpha: " << alpha
		<< "\n Constant r: " << r
		<< "\n Max number of parents: " << np
		<< "\n Pareto(k): " << pareto
		<< endl;
}

void print_CSP(vector<pairs> list) {
	int size = list.size();
	cout << endl << "Constraints: Incompatible Tuples \n";
	//loop through and print all pairs and their tuples
	for (int i = 0; i < size; i++) {
		cout << "(X" << list[i].x1 << ",X" << list[i].x2 << "):";
		for (int j = 0; j < list[i].tuple_list.size(); j++) {
			cout << " " << list[i].tuple_list[j].t1 << ","
				<< list[i].tuple_list[j].t2;
		}
		cout << endl;
	}

	cout << endl;
}

int generateRandomNum(int max) {
	//trying out the built in <random> library
	random_device rd;
	mt19937 eng(rd());
	//min is always 0, max is sent in. 
	//since Max is the total num and we want 0-(max-1) we subtract 1 from max for the range
	uniform_int_distribution<int> distr(0, max - 1);
	int var = distr(eng);

	return var;
}

bool comparePairs(vector<pairs> list, pairs pair) {
	int size = list.size();
	for (int i = 0; i < size; i++) {
		if (list[i].x1 == pair.x1 && list[i].x2 == pair.x2) {
			return true; // there is a match with the pair and a pair in the list
		}
	}
	//we've run through the list and no matches
	return false;
}

bool compareTuples(vector<tuples> list, tuples pair) {
	int size = list.size();
	for (int i = 0; i < size; i++) {
		if (list[i].t1 == pair.t1 && list[i].t2 == pair.t2) {
			return true; // there is a match with the pair and a pair in the list
		}
	}
	//we've run through the list and no matches
	return false;
}

pairs generatePair(int vars) {
	pairs newPair;
	int a = generateRandomNum(vars);
	int b = generateRandomNum(vars);

	//if a == b, need to regen b until not the same
	while (a == b) {
		b = generateRandomNum(vars);
	}
	//to save time sorting(if implemented) and checking, the pairs will be made with the smallest inserted first
	if (a > b) {
		newPair.x1 = b;
		newPair.x2 = a;
	}
	else {
		newPair.x1 = a;
		newPair.x2 = b;
	}

	return newPair;
}

tuples generateTuple(int vars) {
	tuples newTuple;


	int a = generateRandomNum(vars);
	int b = generateRandomNum(vars);

	//we want generated order to stay the same since 1,0 =/= 0,1
	//and 1,1 is a valid tuple constraint
	newTuple.t1 = a;
	newTuple.t2 = b;

	return newTuple;
}

int returnDomain(int vars, double alpha) {
	int domain = round(pow(vars, alpha));

	return domain;
}

//initializes domain of all vars
void createVarDomain(CSP* generated_CSP, int var_num) {
	
	//generate domain
	int domain = generated_CSP->domain;
	for (int i = 0; i < var_num; i++) {
		vector<int> domain_list;
		//create variables object
		variables current_var;
		//loop through domain to fill domain list with all integers in domain.
		//will be same for each var unless arc check is called and prunes them
		for (int j = 0; j < domain; j++) {
			domain_list.push_back(j);
		}
		generated_CSP->var_domains.push_back(domain_list);
	}

	
}
//given list of viable parents and max num parents, return a list of parents
vector<int> generateParents(int num_parent, vector<int> parents) {
	vector<int> parent_list;
	int num;
	//check if list is empty
	if (parents.empty()) {
		//if empty return 0 since will always be x0 here
		parent_list.push_back(0);
	}
	else {
		//if not empty then want to add in parents until parents is exausted or parent_list is full (size = max amount of parents) 
		while (parents.size() > 0 && parent_list.size() <= num_parent) {
			//if list only has one item in it add only that and finish
			if (parents.size() == 1) {
				parent_list.push_back(parents[0]);
				//cout << "Num: " << 0 << " Value: " << parents[0] << endl << endl;;
				break;
			}
			else {
				//generate a num based on current size of vec. 
				num = generateRandomNum(parents.size());
				//put value of that position into CPT
				parent_list.push_back(parents[num]);
				//cout <<"Num: " << num << " Value: " << parents[num] << endl;

				//erase value from domain vec so it isn't used again
				parents.erase(parents.begin() + num);
			}
		}

		//going to want to sort the list for future ease of access when searching
		//examples don't show high num of parents so for sake of time just using swap sort
		int j;
		for (int i = 1; i < parent_list.size(); i++) {
			j = i;
			while (j > 0 && parent_list[j] < parent_list[j - 1]) {
				num = parent_list[j];
				parent_list[j] = parent_list[j - 1];
				parent_list[j - 1] = num;
				j--;
			}
		}
	}



	return parent_list;
}

//function to generate CPT for a var on demand.
//domain is same for all vars until arc check is done so just need it given once
vector<int> generateSingleCPT(int domain) {
	//create a domain in a vector to keep track of what's used
	vector<int> domain_vec;
	for (int i = 0; i < domain; i++) {
		domain_vec.push_back(i);
	}

	//create a vector to hold CPT
	vector<int> CPT;
	int num;

	while (domain_vec.size() > 1) {//number gen does not like max <= 1
		//generate a num based on current size of vec. 
		num = generateRandomNum(domain_vec.size());

		//put value of that position into CPT
		CPT.push_back(domain_vec[num]);
		//cout <<"Num: " << num << " Value: " << domain_vec[num] << endl;
		
		//erase value from domain vec so it isn't used again
		domain_vec.erase(domain_vec.begin() + num);
	}
	//push last remaining num in domain into CPT as lowest priority
	//cout << "Num: " << 0 << " Value: " << domain_vec[0] << endl;
	CPT.push_back(domain_vec[0]);

	return CPT;
}

void generateCPTTables(variables* var, int domain) {
	//if no parent then name = var name and only needs one CPT row
	if (var->name == var->parent_names[0]) {
		var->CPT.push_back(generateSingleCPT(domain));

		//then done
		return;
	}

	//if there is a parent, need to create domain ^ parents to cover all permutations
	int permutations = pow(domain, var->parent_names.size());
	for (int i = 0; i < permutations; i++) {
		var->CPT.push_back(generateSingleCPT(domain));
	}

	return;
}

void printCPT(vector<variables>* vars, int domain) {
	cout << "Dependancies: List of parents for each variable:" << endl;
	cout << "X0: Nil \n";
	vector<int>* parents;
	vector<vector<int>>* CPT;
	//print parent lists
	for (int i = 1; i < vars->size() ; i++) {
		cout << "X" << (*vars)[i].name << ": {";
		parents = &(*vars)[i].parent_names;
		for (int j = 0; j < parents->size(); j++) {
			cout << "X" << (*parents)[j];
			if (j == parents->size() - 1) {
				cout << "} \n";
			}
			else {
				cout << ",";
			}
		}
	}

	//print CPT lists
	cout << "CPT for each variable: \n";
	for (int i = 0; i < vars->size(); i++) {
		cout << "X" << (*vars)[i].name << ": \n";
		printCPPList(&(*vars)[i], domain);
	}
}

void printCPPList(variables* var, int domain) {
	//we don't know number of parents per run so put par1, par2,...parn in a list
	vector<int> parents;
	int num_parents = var->parent_names.size();
	int permutations = pow(domain, num_parents);
	for (int i = 0; i < num_parents; i++) {
		parents.push_back(0);
	}

	if (parents[0] == var->name) { //catch x0 which has no parent
		for (int j = 0; j < domain; j++) {
			//print CPT 
			cout << var->CPT[0][j];
			if (j != domain - 1) {
				cout << ">";
			}
		}
		cout << endl;
	}
	else {
		for (int i = 0; i < permutations; i++) {
			for (int k = 0; k < num_parents; k++) {
				cout << parents[k];
				if (k != num_parents - 1) {
					cout << ",";
				}
				else {
					cout << ": ";
				}
			}
			for (int j = 0; j < domain; j++) {
				//print CPT 
				cout << var->CPT[i][j];
				if (j != domain - 1) {
					cout << ">";
				}
			}
			cout << endl;
			parents[num_parents - 1]++;
			//need to increment parents to show all permutations
			for (int k = num_parents - 2; k >= 0; k--) {
				if (parents[k + 1] > domain - 1) {
					parents[k + 1] = 0;
					parents[k]++;
				}
			}
		}
	}
}
