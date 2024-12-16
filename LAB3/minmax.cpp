#include <iostream>
using std::cout;
using std::cin;
using std::endl;
#include <string>
using std::string;
#include <fstream>
using std::ifstream;
// #include "mpi.h"

int main () {

  // Declare input file variable to hold input file
  string inputFileName;

  // Declare double variable to store the next value we read
  double nextValue;

  // Declare double variables to keep track of min and max
  double maxValue, minValue;

  // Declare input filestream objects (read only)
  ifstream inFile;

  // Ask user for input file
  cout << "Please enter the file you want me to find the max/min for: " << endl;

  // Take in input file
  cin >> inputFileName;

  // Open data file
  inFile.open(inputFileName);

  //Check for file opening errors
  if(inFile.fail()){
    cout << "Error opening file: " + inputFileName << endl;
    exit(0);
  }

  // Loop to read each item from file while it has a next value
  while(inFile >> nextValue){

    // Check if next value if min/max and update accordingly
    if(nextValue > maxValue) { maxValue = nextValue; }
    if(nextValue < minValue) { minValue = nextValue; }
  }

  // Close file
  inFile.close();

  cout << "Minimum: " << minValue << endl;
  cout << "Maximum: " << maxValue << endl;

  return 0;
}
