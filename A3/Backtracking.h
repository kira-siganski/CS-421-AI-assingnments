#pragma once
#include "Structures.h"
#include <vector>
using namespace std;

//main function for standard backtrack
void backtrackSearch(CSP* csp_instance, int pareto);

vector<int> backtrackSearchRecursion(CSP* csp_instance, vector<int> current_node);

bool constraintCheck(CSP* csp_instance, vector<int> node);

void printResult(vector<int> node);