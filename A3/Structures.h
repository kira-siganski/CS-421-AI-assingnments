#pragma once
#pragma once
#include <vector>
#include <cmath>

using namespace std;
//struct to hold tuples 
struct tuples {
	int t1;
	int t2;
};
//struct with to hold pairs and their list of tuples
struct pairs {
	int x1;
	int x2;
	vector<tuples> tuple_list;
};

//to hold a variable by defninig its parents and CPT preferences
struct variables {
	int name;
	vector<int> parent_names;
	//vector size here will correspond to domain^parent_names.size() for this var
	//see bottom of this file for logic with CPT list and finding which parent values correspond to what number in the list
	vector<vector<int>> CPT;
};



//struct to hold entire CSP and CPT instance
struct CSP {
	//vector of CSP pairs
	vector<pairs> constraints;
	//vector of variables. each var holds it's CPT and related parents
	vector<variables> vars;
	//holds domains of vars. need to be able to make copy of this fo FLA
	vector<vector<int>> var_domains;
	int domain;

	//container for all found solutions
	vector<vector<int>> solutions;

	
	//find location of parent CPT based on set values of parents
	int findParentCPT(vector<int> parent_values) {
		int num_parents = parent_values.size();
		int location = 0; //need to initialise this
		//sum Xi*domain^(num_parents - i) in location
		int power;
		for (int i = num_parents; i > 0; i--) {
			power = num_parents - i;
			location += parent_values[i - 1] * pow(domain, power);
		}
		return location;
	}
	//find the preference weight of a given solution
	int findWeight(vector<int> node) {
		//for each set var, find it's weight based on prefered value
		//if value set matches value in CPT[0] for that var, value is highest
		int location;
		int weight = 0;
		for (int i = 0; i < node.size(); i++) {
			//given var i, find it's corresponding CPT
			//if var i = var0, there is no parent so location needs to be 0
			if (i == 0) {
				location = 0;
			}
			else {
				location = findParentCPT(vars[i].parent_names);
			}
			
			for (int j = domain; j > 0; j--) {
				//traverse through xi's CPT and weight = domain - position
				if (node[i] == vars[i].CPT[location][j - 1]) {
					weight += j;
				}
			}
		}

		return weight;
	}
	//find weight max location for utility function
	int findWeightMaxLocation(vector<int>* weightList) {
		int max = (*weightList)[0];
		int location = 0;
		for(int i = 0; i < (*weightList).size(); i++) {
			if ((*weightList)[i] > max) {
				max = (*weightList)[i];
				location = i;
			}

		}
		return location;
	}
};

/*3 parents with domain of 4 means 
4^3 = 64 permutations

x1 x2 x3
000
001
etc

x3 has a weight of 1
x2 = 1 means 4 x3's have happened therefore weight of 4

x1 = 1 means 4x2s have happened there for 4x4 = weight 16

==> position in CPT = x3 + x2*4 + x1 * 16
==> position = summation: i=0, i<num_parents, Xi*domain^(num_parents - i) where 0 < i < num_parents
so I can hold an array of preferences and only have to calculate postion in array based on parent values
*/
