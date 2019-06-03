#ifndef TarjanSoftHeap_Included
#define TarjanSoftHeap_Included

struct Item {
  Item *next;
  int key;
};

struct Node {
  /* Next root if node is a root. */
  Node *next;
  Node *left, *right;

  /* Points to last item in item set. */
  Item *set;

  int key;
  int rank;
};

class TarjanSoftHeap {
public:
  TarjanSoftHeap();
  ~TarjanSoftHeap();

  /* Public Methods. */
  void insert(int key);
  int findMin();
  int deleteMin();
  void meld(Node *h);

private:
  /* Private Methods. */
  void doubleEvenFill(Node *x);
  void fill(Node *x);
  Node *rankSwap(Node *h);
  Node *keySwap(Node *h);
  Node *reorder(Node *h, int k);
  Node *makeRoot(Item *e);
  Node *link(Node *x, Node *y);
  Node *meldableInsert(Node *x, Node *h);
  Node *meldableMeld(Node *h1, Node *h2);

  /* Private Data. */
  Node *heap;
  Node *Null;

  int t;
};

#endif
