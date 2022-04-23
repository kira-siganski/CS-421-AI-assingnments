#include "FLA.h"
#include <iostream>


void FLASearch(CSP* csp_instance, int pareto) {
	cout << "Full Look Ahead chosen \n";

	//create root node.
	//root is no values assigned

	//perform initial arc check. Asking if a user wants an arc check is redunant for this search type
	arcConsistency(csp_instance);
	int num = 0;
	//we want this to run until all possible outcomes are exhausted
	while (!(*csp_instance).var_domains[0].empty()) {
		vector<int> root;

		//now we send this out
		root = FLArecursion(csp_instance, &csp_instance->var_domains, root);
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

vector<int> FLArecursion(CSP* csp_instance, vector<vector<int>>* var_domains, vector<int> node) {
	//now when we assign a variable, we first prune the domain of it, then prune the domain after it is assigned
	//so call arc check before assigning, then after assigning
	
	//find out which node we're on
	//if list is full, then last node has been checked and all values assigned work so return list
	vector<variables>* var_list = &csp_instance->vars;
	if (node.size() == (*var_list).size()) {
		return node;
	}

	int var_num = node.size(); //if node size == var size returns node, then var_num will never be greater than varlist size or equal here
	vector<vector<int>> child_var_list = *var_domains;
	int domain = (*var_domains)[var_num].size(); //domain size of current node to be explored
	vector<int> node_child;

	//if we somehow have an empty domain, return fail state
	if (domain == 0) {
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

	//now we assign a var
	for (int i = 0; i < domain; i++) {
		//node[var_num] = var_domains[var_num][i]; //assign a value in Xvar_num's domain
		//NEW now that we have CPTs, we want to grab the most wanted one based on any assigned parent vars if applicable
		node[var_num] = (*csp_instance).vars[var_num].CPT[parent_position][i];

		//need to ensure the value grabbed here is in the domain of the var
		if (checkDomain(&(*var_domains)[var_num], node[var_num])) {
			//arc check on this assignment
			child_var_list = FLCarcCheck(csp_instance, var_domains, node);
			//then check if any domains have a size of 0
			//if not then explore this child
			if (checkDomainSize(&child_var_list)) {
				node_child = FLArecursion(csp_instance, &child_var_list, node);
				//if child path succeeds return child
				if (node_child[0] != -1) {
					return node_child;
				}
			}
			//otherwise we try next value in domain and loop again
		}
		
	}

	//if we're here than no child succeeded so return fail
	node[0] = -1;
	return node;
}

vector<vector<int>> FLCarcCheck(CSP* csp_instance, vector<vector<int>>* var_domains, vector<int> node) {
	//similar to arc check but we take into account the assigned var.
	//since this is called each time a value is assigned, this check will only take into account the last node if it exists in a tuple
	//then we use that to prune the domains further each time the alg goes to a deeper depth

	//so also simular to Forwardchecking since we update based on newest assignment/child
	vector<pairs>* csp_pairs = &csp_instance->constraints;
	vector <vector<int>> child_domains = (*var_domains);
	int size = node.size() - 1; //also denotes current node/latest assignment
	int constraint_size = csp_pairs->size();
	int x1, x2, t1, t2;

	//iterate through pair list to find any that have the current child in
	for (int i = 0; i < constraint_size; i++) {
		x1 = (*csp_pairs)[i].x1;
		x2 = (*csp_pairs)[i].x2;

		//check if x1 is currently in any of these domains
		if (x1 == size || x2 == size) {
			//only need to ensure if any tuple containing node's latest child's assigned value exits, remove the corresponding value from the other x's domain
			for (int j = 0; j < (*csp_pairs)[i].tuple_list.size(); j++) {
				t1 = (*csp_pairs)[i].tuple_list[j].t1;
				t2 = (*csp_pairs)[i].tuple_list[j].t2;

				//since location matters, we check if child is on one side or other
				//we also don't want to change the domain of previously set numbers. Only want this to affect future domain so backtracking isnt screwed up
				if (size == x1 && node[size] == t1 && x2 > size) {
					//if we've found a tuple, we want to prune it from the other's domain
						pruneDomain(&child_domains, x2, t2);
				}
				else if (size == x2 && node[size] == t2 && x1 > size) {
					//same here just flip direction
						pruneDomain(&child_domains, x1, t1);
				}
			}
		}
	}

	//if we reach this point then all possible changes to domain have happened
	return child_domains;
}


//check if a domain is empty, is so this branch is dead
bool checkDomainSize(vector<vector<int>>* var_list) {
	//loop through var list and check domain list size
	int size = var_list->size();

	for (int i = 0; i < size; i++) {
		//if domain size is 0 then empty and return false for fail state
		if ((*var_list)[i].size() == 0) {
			return false;
		}
	}

	//if we exit the loop then no empty domain found
	return true;
}

//ensure CPT value is in domain
bool checkDomain(vector<int>* var_domain, int value) {
	bool inDomain = false;
	for (int i = 0; i < var_domain->size(); i++) {
		if ((*var_domain)[i] == value) {
			inDomain = true;
		}
	}

	return inDomain;
}