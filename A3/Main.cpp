//made for CS421 A3 winter semester 2022
//Kira Siganski 200352511

#include <iostream>
#include <vector>

#include "CSPgen.h"
#include "Structures.h"
#include "ArcConsistency.h"
#include "Backtracking.h"
#include "ForwardChecking.h"
#include "FLA.h"

using namespace std;
void navigateToSearch(CSP* csp_container, int search_type, char arc_choice, int pareto);

int main() {
	bool check = true;
	char user_choice, arc_choice;
	int var_constraint, search_type, num_parents, num_pareto;
	double tightness_constraint, alpha_constraint, r_constraint;
	CSP* csp_container;

	do {
		//first step, generate the binary CSP 
		//we need input for n, p, alpha and r
		cout << "Please enter a positive integer value for n: ";
		cin >> var_constraint;
		cout << "Please enter a value for the tightness p where 0 < p < 1: ";
		cin >> tightness_constraint;
		cout << "Please enter a value for alpha where 0 < alpha < 1: ";
		cin >> alpha_constraint;
		cout << "Please enter a value for r where 0 < r < 1: ";
		cin >> r_constraint;
		cout << "Please enter a positive integer value for the max number of parents: ";
		cin >> num_parents;
		cout << "Please enter a positive integer value for the max number of solutions(pareto): ";
		cin >> num_pareto;

		//test instance so I don't have to keep typing in same vars
		//var_constraint = 4;
		//tightness_constraint = 0.33;
		//alpha_constraint = 0.8;
		//r_constraint = 0.7;
		//num_parents = 3;
		//num_pareto = 3;

		//generate the binary CSP and print
		csp_container = generateCSP(var_constraint, tightness_constraint, alpha_constraint, r_constraint, num_parents, num_pareto);

		//what search type are we using? 
		// BT = 0, FC = 1 FLA = 2
		cout << "Which search would you like to use? \n"
			<< "Enter 0 for Backtracking, \n"
			<< "Enter 1 for Forward Checking, \n"
			<< "Enter 2 for Full Look Ahead. \n";
		cin >> search_type;

		//do we want to run Arc Consistency check before search?
		cout << "Would you like to run the Arch Consistency Check before the search? \n"
			<< "Enter y for yes or n for no: ";
		cin >> arc_choice;

		navigateToSearch(csp_container, search_type, arc_choice, num_pareto);
		
		//do we want to run generate and search another binaryCSP?
		cout << "\n\nWould you like to make another binary CSP? \n Enter y or enter n to stop: ";
		cin >> user_choice;
		cout << endl;
		if (user_choice == 'n' || user_choice == 'N') {
			check = false;
		}
	} while (check);


	return 0;
}

void navigateToSearch(CSP* csp_container, int search_type, char arc_choice, int pareto) {
	//check if arc consistency is requested
	switch (arc_choice) {
		case 'y':
		case 'Y':
			cout << "\nArc Consistency Check chosen. \n";
			arcConsistency(csp_container);
			arcPrintTest(csp_container);

			break;
		case 'n':
		case 'N':
			cout << "\nNo Arc Consistency Check. \n";
			break;
	}

	//send the generated csp off to the right search type
	switch (search_type) {
		case 0: //Backtrack search
			backtrackSearch(csp_container, pareto);
			break;
		case 1: //Forward Checking
			forwardCheckSearch(csp_container, pareto);
			break;
		case 2: //full look ahead
			FLASearch(csp_container, pareto);
			break;
	}
}
