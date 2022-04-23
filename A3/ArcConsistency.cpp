#include "ArcConsistency.h"
void arcConsistency(CSP* csp_container) {
	//we want to prune the domain of each var if, for constraint X1,X2, for every value a where a is an integer in domain X1,
	//if there exists tuples for pair X1, X2: a,b1 a,b2... a,bn where all values in b cover every possible value in the domain of X2
	//then value a from X1's domain must be removed from the domain of X1
	int domain_size = csp_container->domain;
	int tuple_num = csp_container->constraints[0].tuple_list.size();
	vector<pairs>* tuple_pairs = &csp_container->constraints;
	vector<vector<int>>* var_domains = &csp_container->var_domains;
	vector<int> prune_list_X1, prune_list_X2;
	//iterate through tuple list.
	int t1, t2, x1, x2;
	for (int i = 0; i < tuple_num; i++) {
		//initialize our list for each search since we are using a bucket sort method
		//list is size of domain so list[a] is the number of times num a in domain X1 shows up for tuple X1, X2
		prune_list_X1 = initializeList(prune_list_X1, domain_size);
		prune_list_X2 = initializeList(prune_list_X2, domain_size);
		//direction matters, check for X1 and check for X2's side 
		for (int j = 0; j < (*tuple_pairs)[i].tuple_list.size(); j++) {
			t1 = (*tuple_pairs)[i].tuple_list[j].t1;
			t2 = (*tuple_pairs)[i].tuple_list[j].t2;
			prune_list_X1[t1] += 1;
			prune_list_X2[t2] += 1;
		}
		x1 = (*tuple_pairs)[i].x1;
		x2 = (*tuple_pairs)[i].x2;
		//now that we've gone through the entirel tuple set we prune if needed
		for (int k = 0; k < domain_size; k++) {
			if (prune_list_X1[k] == domain_size) {
				pruneDomain(var_domains, x1, k);
			}
			if (prune_list_X2[k] == domain_size) {
				pruneDomain(var_domains, x2, k);
			}
		}
	}
}

//since our list is of variable size per run based on domain size, this is a utility function to initialize it and reset it to zero
vector<int> initializeList(vector<int> list, int size) {
	//if list has items, clear it
	if (list.size() > 0) {
		list.clear();
	}
	
	for (int i = 0; i < size; i++) {
		list.push_back(0);
	}

	return list;
}

void pruneDomain(vector<vector<int>>* var_list, int var, int domain_var) {
	//since this can be called multiple times on the same domain, does not ensure our domain for selected var is complete
	//thus loop through domain list until domain_var is found then erase it from the vector
	int size = (*var_list)[var].size();
	//site that loop code is from. helped me figure out vector iterators since apparently I forgot https://linuxhint.com/remove-specific-element-vector-cpp/#:~:text=How%20do%20I%20remove%20a%20Specific%20Element%20from,The%20C%2B%2B%20vector%20has%20many%20member%20functions.%20
	for (vector<int>::iterator it = (*var_list)[var].begin(); it != (*var_list)[var].end(); it++) {
		if (*it == domain_var) {
			(*var_list)[var].erase(it);
			break;
		}
	}
}

void arcPrintTest(CSP* csp_container) {
	//print test to see if pruned properly. uncomment to check
	int var_num = csp_container->vars.size();
	vector<vector<int>>* var_domains = &csp_container->var_domains;
	for (int i = 0; i < var_num; i++) {
		//print test 
		cout << endl;
		cout << "Domain of: X" << i << endl;
		for (int j = 0; j < (*var_domains)[i].size(); j++) {
			cout << (*var_domains)[i][j] << ", ";
		}
		cout << endl;
	}
}