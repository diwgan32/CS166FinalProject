#include "LCRSSoftHeap.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
using namespace std;

int main() {
  LCRSSoftHeap sh;
  string instruction;
  ifstream instructions("randomInput.txt");
  int numInsertions = 0, numDeletions = 0;
  if (instructions.is_open()) {
    while (getline(instructions, instruction)) {
      if (instruction[0] == 'i') {
        int toInsert = atoi(instruction.c_str() + 2);
        sh.insert(toInsert);
        numInsertions++;
        cout << "Inserted " << toInsert << "." << endl;
      } else if (instruction[0] == 'd') {
        int deleted = sh.deleteMin();
        int diff = deleted < numDeletions ? numDeletions - deleted : deleted - numDeletions;
        cout << "Deleted " << deleted << "." << endl;
//        cout << "Distance from true rank: " << diff << endl;
        numDeletions++;
      } else if (instruction[0] == 'p') {
        // sh.print("");
      }
    }
    instructions.close();
  }
}
