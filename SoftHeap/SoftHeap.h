#ifndef SoftHeap_Included
#define SoftHeap_Included

#include <vector>
#include <string>
using std::vector;  using std::string;

typedef struct ILCELL {
  int key;
  struct ILCELL *next;
} ILCell;

typedef struct NODE {
  int ckey, rank;
  struct NODE *next, *child;
  struct ILCELL *il, *ilTail;
} Node;

typedef struct HEAD {
  struct NODE *queue;
  struct HEAD *next, *prev, *suffixMin;
  int rank;
} Head;

typedef struct BINTREE {
  int ckey;
  struct ILCELL *il, *ilTail;
  vector<struct BINTREE *> children; 
} BinTree;

class SoftHeap {
public:
  SoftHeap();
  ~SoftHeap();

  /* Public Methods. */
  void insert(int key);
  int deleteMin();

  /* Printing methods. */
  void print(string spaces);
  void printAsBinomialTrees(string spaces);

private:
  /* Private Methods. */
  void meld(Node *q);
  void fixMinList(Head *h);
  Node *sift(Node *v, string spaces);

  /* Methods for printing the soft queues. */
  void printQueue(Node *n, int depth, string spaces);
  void printQueueAsBinomialTree(Node *n, int depth);
  void createBinomialTree(Node *n, BinTree *parent);
  string createBinTreeStrings(vector<string>& levels, int depth, 
          string baseSpaces, string secSpaces, BinTree *node);

  /* Private Data. */
  Head *header, *tail;
  int r;
};

#endif
