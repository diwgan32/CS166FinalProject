#ifndef LCRSSoftHeap_Included
#define LCRSSoftHeap_Included

#include <string>
#include <vector>
typedef struct ILCELL {
  int key;
  struct ILCELL *next;
} ILCell;

typedef struct NODE {
  int ckey, rank;
  struct NODE *lc, *rs;
  struct ILCELL *il, *ilTail;
} Node;

typedef struct HEAD {
  struct NODE *queue;
  struct HEAD *next, *prev, *suffixMin;
  int rank;
} Head;

class LCRSSoftHeap {
public:
  LCRSSoftHeap();
  ~LCRSSoftHeap();

  /* Public Methods. */
  void insert(int key);
  int deleteMin();
private:
  /* Private Methods. */
  void meld(Node *q);
  void fixMinList(Head *h);
  Node *sift(Node *v);
  
  /* Private Data. */
  Head *header, *tail;
  int r;
};

#endif
