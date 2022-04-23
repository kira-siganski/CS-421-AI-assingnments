#include "Backtracking.h"
#include <iostream>


/*
assign x1 and generate children for possible x2 in domain with x1
check if first child works, if yes, then explore the first grandchild that works
if no solution in a node's children check next sibling*/
void backtrackSearch(CSP* csp_instance, int pareto) {
	cout << "backtrack search selected";
	
	int num = 0;
	//we want this to run until all possible outcomes are exhausted
	while (!(*csp_instance).var_domains[0].empty()) {
		vector<int> root;

		//now we send this out
		root = backtrackSearchRecursion(csp_instance, root);
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

//recursive part of search
vector<int> backtrackSearchRecursion(CSP* csp_instance, vector<int> node) {
	//we're given a node with assigned variables, if first var = -1 then check next child.
	//if no more children to check, assign -1 to first val and return node
	
	//find out which node we're on
	//if list is full, then last node has been checked and all values assigned work so return list
	if (node.size() == csp_instance->vars.size()) {
		return node;
	}

	int var_num = node.size(); //if node size == var size returns node, then var_num will never be greater than varlist size or equal here
	int domain = csp_instance->domain;
	int parent_position;
	bool fits_constraints = true;
	vector<int> node_child; //for exploring a child, and to check if child returns valid solution without altering our current node if child[0] == -1
	//set value as first in domain for current X or next in list
	//if we somehow have an empty domain, return fail state
	if (domain == 0) {
		node.push_back(-1); //catch if empty list
		node[0] = -1;
		return node;
	}

	node.push_back(0); //could be any number, just want to make sure node[var_num] exists since it wouldn't before now

	//check if any parents are assigned, if so then find the location of the appropriate CPT for this node
	variables* current_var = &(*csp_instance).vars[var_num];
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
	//generate children of node and call recursion on each one
	for (int i = 0; i < domain; i++) {
		//node[var_num] = (*csp_instance).var_domains[var_num][i]; //assign a value in Xvar_num's domain
		//NEW now that we have CPTs, we want to grab the most wanted one based on any assigned parent vars if applicable
		node[var_num] = (*csp_instance).vars[var_num].CPT[parent_position][i];

		//if assigning a value to any X after X1, check if value fits constraints
		if (var_num > 0) {
			fits_constraints = constraintCheck(csp_instance, node);
		}
		//if current assingments fit within constraints explore this child, otherwise move on to next one
		if (fits_constraints) {
			node_child = backtrackSearchRecursion(csp_instance, node);
			if (node_child[0] != -1) { //if exploring this child yeilds a result, return result and stop recursion
				return node_child;
			}
			//if not we reassign node[var_num] until domain is exhausted
		}
	}
	//if we reach this section, all children failed to produce a valid combination
	//so set node[0] = -1 and return fail state
	node[0] = -1;
	return node;
}

//check if value fits constraints
bool constraintCheck(CSP* csp_instance, vector<int> node) {
	//keep track of size, if current values assigned don't cover all constraint pairs, ignore pairs with values unassigned
	int size = node.size() - 1;
	int constraint_size = (*csp_instance).constraints.size();
	vector<pairs>* var_list = &csp_instance->constraints;
	int x1, x2, t1, t2;

	for (int i = 0; i < constraint_size; i++) {
		x1 = (*var_list)[i].x1;
		x2 = (*var_list)[i].x2;

		if (x1 <= size && x2 <= size) {
			//if pair covers Xa, Xb where Xa and Xb are assigned values, then ensure assigned pair doesn't match any tuple
			for (int j = 0; j < (*var_list)[i].tuple_list.size(); j++) {
				t1 = (*var_list)[i].tuple_list[j].t1;
				t2 = (*var_list)[i].tuple_list[j].t2;

				if (node[x1] == t1 && node[x2] == t2) {
					return false;
				}
			}
		}
	}
	//we reach this point then no tuple pair for Xa, Xb matched assigned values so node fits constraints
	return true;
}


//print result
void printResult(vector<int> node) {
	cout << endl;
	if (node[0] == -1) {
		cout << "no valid solutions found";
	}
	else {
		cout << "Possible solution: ";
		int size = node.size();
		for (int i = 0; i < size; i++) {
			cout << "X" << i << " = " << node[i];
			if (i != size - 1) {
				cout << ", ";
			}
		}
	}

}

