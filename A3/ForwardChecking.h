#pragma once
#include "Structures.h"
#include "Backtracking.h" //for print function
#include <vector>
using namespace std;

//main function for Forward Checking
void forwardCheckSearch(CSP* csp_instance, int pareto);

vector<int> forwardCheckRecursion(CSP* csp_instance, vector<int> node);

bool FCconstraintCheck(CSP* csp_instance, vector<int> node);