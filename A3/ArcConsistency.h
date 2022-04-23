#pragma once
#include "Structures.h"
#include <vector>
#include<iostream>
using namespace std;

//prune the domain of our generated CSP for each variable
void arcConsistency(CSP* csp_container);

vector<int> initializeList(vector<int> list, int size);

void pruneDomain(vector<vector<int>>* var_list, int var, int domain_var);

void arcPrintTest(CSP* csp_container);