Kira Siganski
200352511
- A2 is the base for A3 so putting up A3 as the completed version of this program

compiling:
Console commands to input once all files are in the same directory (should work this time for sure. turns out visual studio is not as case sensitive for file names)


make
./assign3

program will run and ask you to input variables seperately in this order:
n
p
alpha
r
max number of parents
pareto - max number of optimal slns to display
The generated instance will be printed out

then program will ask you to input '1','2' or '3' to select your search type
'y' or 'n' for arc consistency check

Up to the number of inputed pareto slns will be printed. Sometimes less if less than the pareto amount exists for the generated problem

then program will ask for 'y' or 'n' if you want to generate another instance and search it.

if you want to clean up the compiled .o files, type

make clean

into the console
