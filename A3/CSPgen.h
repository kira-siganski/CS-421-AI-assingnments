#pragma once
#include "Structures.h"
#include <vector>
#include <string>

using namespace std;

//main function we send the vars to
CSP* generateCSP(int vars, double tightness, double alpha, double r, int num_parent, int pareto);
/*print input givven to generate CSP
input is vars, tightness, alpha, r*/
void print_input(int vars, double tightness, double alpha, double r, int np, int pareto);

void print_CSP(vector<pairs> list);

//return a random integer within range [0,max)
int generateRandomNum(int max);

//check if a pair already exists in a list
bool comparePairs(vector<pairs> list, pairs pair);

//check if a pair already exists in a list
bool compareTuples(vector<tuples> list, tuples pair);

//return a pairs struct of two generated variable pairs
pairs generatePair(int vars);

//return a tuples struck holding the generated tuple pair
tuples generateTuple(int vars);

//utility function to find domain size
int returnDomain(int vars, double alpha);

//moving this here so calcs aren't done in main, changed to only initialize var domains now that we have a CSP container
void createVarDomain(CSP* generated_CSP, int var_num);

//pick random parents for each var based on list of avalable parents and max num parents allowed
vector<int> generateParents(int num_parent, vector<int> parents);

//generate a single CPT based on domain size
vector<int> generateSingleCPT(int domain);

//generate CPTs based on all possible parent values
//cpt list will get large so pass by reference
void generateCPTTables(variables* var, int domain);

void printCPT(vector<variables>* vars, int domain);

void printCPPList(variables* var, int domain);