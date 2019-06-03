#include "LCRSSoftHeap.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <chrono>
using namespace std;

int runTest() {
  LCRSSoftHeap sh;
  for (int i = 0; i < 10000; i++) {
    sh.insert(i);
  }
  for (int i = 0; i < 10000; i++){
    sh.deleteMin();
  }
}

int main() {
  LCRSSoftHeap sh;
  string instruction;
  ifstream instructions("intput.txt");
  if (instructions.is_open()) {
    while (getline(instructions, instruction)) {
      /* Run tests. */
      if (instruction[0] == 't') {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 10000; i++) {
          runTest();
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Elapsed time: " << elapsed.count() << " s\n";
        break;
      } 
      /* Insert an item. */
      else if (instruction[0] == 'i') {
        int toInsert = atoi(instruction.c_str() + 2);
        sh.insert(toInsert);
        cout << "Inserted " << toInsert << "." << endl;
      } 
      /* Delete an item. */
      else if (instruction[0] == 'd') {
        int deleted = sh.deleteMin();
        cout << "Deleted " << deleted << "." << endl;
      }
    }
    instructions.close();
  }
}
