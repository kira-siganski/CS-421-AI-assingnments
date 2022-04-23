#include "ForwardChecking.h"
#include <iostream>

void forwardCheckSearch(CSP* csp_instance, int pareto) {
	cout << "Forward Checking Search Chosen \n";
	int num = 0;
	//we want this to run until all possible outcomes are exhausted
	while (!(*csp_instance).var_domains[0].empty()) {
		vector<int> root;

		//now we send this out
		root = forwardCheckRecursion(csp_instance, root);
		(*csp_instance).var_domains[0].erase((*csp_instance).var_domains[0].begin());
		if (csp_instance->solutions.empty()) {
			csp_instance->solutions.push_back(root);
		}
		if (root != (*csp_instance).solutions[num] && root[0] != -1) {
			csp_instance->solutions.push_back(root);
			num++;
		}

	}
	vector<int> root;
	//if we have more solutions than pareto max, need to cull the lowest weighted ones
	if (csp_instance->solutions.size() > pareto) {
		//grab bucket sort of all the weights
		vector<int> weights;
		int weight;
		for (int i = 0; i < csp_instance->solutions.size(); i++) {
			weight = csp_instance->findWeight((*csp_instance).solutions[i]);
			weights.push_back(weight);
		}

		vector<vector<int>> pareto_maxes;
		int max_position = csp_instance->findWeightMaxLocation(&weights);

		while (pareto_maxes.size() < pareto) {
			//add best weighted CPT solution to the list
			pareto_maxes.push_back((*csp_instance).solutions[max_position]);
			//remove that one from the solutions list
			(*csp_instance).solutions.erase((*csp_instance).solutions.begin() + max_position);
			weights.erase(weights.begin() + max_position);
			//find next best
			max_position = csp_instance->findWeightMaxLocation(&weights);
		}

		//print root
		for (int i = 0; i < csp_instance->solutions.size(); i++) {
			root = pareto_maxes[i];
			printResult(root);
		}
	}
	else {

		//print root
		for (int i = 0; i < csp_instance->solutions.size(); i++) {
			root = (*csp_instance).solutions[i];
			printResult(root);
		}
	}
}

vector<int> forwardCheckRecursion(CSP* csp_instance, vector<int> node) {
	//when we assign a variable here, we want to ensure it is consistent with constraints
	//so our constraint check is done before assignment
	vector<variables>* var_list = &csp_instance->vars;
	if (node.size() == (*var_list).size()) { //end state if all values are assigned
		return node;
	}

	int var_num = node.size(); //if node size == var size returns node, then var_num will never be greater than varlist size or equal here
	int domain = csp_instance->domain;
	vector<int> node_child;

	if (domain == 0) {//catch fail state of empty domain
		node.push_back(-1); //catch if empty list
		node[0] = -1;
		return node;
	}

	node.push_back(0); //initialize our var

	//check if any parents are assigned, if so then find the location of the appropriate CPT for this node
	variables* current_var = &(*csp_instance).vars[var_num];
	int parent_position;
	if (var_num == 0) {
		//0 never has any parents so set position as 0
		parent_position = 0;
	}
	else {
		int parent_num = current_var->parent_names.size();
		vector<int> parents;
		vector<int> parent_vals;
		for (int i = 0; i < parent_num; i++) {
			parents.push_back(current_var->parent_names[i]);
		}
		//since parents are in order of smallest i xi to largest xi, we can use these values to find and push back the vals of set parents
		//parents will always be set since a node cannot have a parent with a larger number i than it
		for (int j = 0; j < node.size(); j++) {
			//check if node j is in the parent list
			for (int k = 0; k < parents.size(); k++) {
				if (parents[k] == j) {
					parent_vals.push_back(node[j]);
				}
			}
		}

		parent_position = csp_instance->findParentCPT(parent_vals);
	}
	//generate child, check if child fits constraints, move on if it doesnt, search if it does
	//child is node[size of parent], parent is node where 0-(size of parent -1) is parent node
	for (int i = 0; i < domain; i++) {
		//node[var_num] = (*csp_instance).var_domains[var_num][i]; //set first child
		//NEW now that we have CPTs, we want to grab the most wanted one based on any assigned parent vars if applicable
		node[var_num] = (*csp_instance).vars[var_num].CPT[parent_position][i];
		
		//now check if assignment fits constraints
		if (FCconstraintCheck(csp_instance, node)) {
			//if fits constraints, explore node further
			node_child = forwardCheckRecursion(csp_instance, node);
			if (node_child[0] != -1) { //if exploring this child yeilds a result, return result and stop recursion
				return node_child;
			}
			//if no result from exploring that branch, ignore it and assign new value
		}
	}

	//if we've exhausted the loop then this node is not viable
	//return fail state
	node[0] = -1;
	return node;
}


bool FCconstraintCheck(CSP* csp_instance, vector<int> node) {
	//similar to standard backtrack, only we only care about constraint pairs
	//if last node is in them
	
	//keep track of size, if current values assigned don't cover all constraint pairs, ignore pairs with values unassigned
	vector<pairs>* csp_pairs = &csp_instance->constraints;
	int size = node.size() - 1;
	int constraint_size = (*csp_pairs).size();
	int x1, x2, t1, t2;

	for (int i = 0; i < constraint_size; i++) {
		x1 = (*csp_pairs)[i].x1;
		x2 = (*csp_pairs)[i].x2;

		//first make sure pair covers currently assigned values
		if (x1 <= size && x2 <= size) {
			//now make sure pair includes child
			if (x1 == size || x2 == size) {
				//if pair covers Xa, Xb where Xa and Xb are assigned values, then ensure assigned pair doesn't match any tuple
				for (int j = 0; j < (*csp_pairs)[i].tuple_list.size(); j++) {
					t1 = (*csp_pairs)[i].tuple_list[j].t1;
					t2 = (*csp_pairs)[i].tuple_list[j].t2;

					if (node[x1] == t1 && node[x2] == t2) {
						return false;
					}		
				}
			}
		}

	}
	
	//if we reach this point then child fits constraints
	return true;
}