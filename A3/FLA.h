#pragma once
#include "Structures.h"
#include "Backtracking.h" //for print function
#include "ArcConsistency.h" //we want both the default arc check and the prune function
#include <vector>
using namespace std;

//main function for Full look ahead
void FLASearch(CSP* csp_instance, int pareto);

vector<int> FLArecursion(CSP* csp_instance, vector<vector<int>>* var_domains, vector<int> node);

vector<vector<int>> FLCarcCheck(CSP* csp_instance, vector<vector<int>>* var_domains, vector<int> node);

bool checkDomainSize(vector<vector<int>>* var_list);

bool checkDomain(vector<int>* var_domain, int value);