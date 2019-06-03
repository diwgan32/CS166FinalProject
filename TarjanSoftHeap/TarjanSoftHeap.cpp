#include "TarjanSoftHeap.h"
#include <limits>
#include <iostream>
#include <utility>
#include <cmath>
using namespace std;

TarjanSoftHeap::TarjanSoftHeap() {
  Null = new Node();
  Null->next = Null->left = Null->right = Null;
  Null->set = NULL;
  Null->key = Null->rank = numeric_limits<int>::max();
  
  heap = Null;
  t = ceil(log2(3.0 / .1875));
}

TarjanSoftHeap::~TarjanSoftHeap() { }

void TarjanSoftHeap::doubleEvenFill(Node *x) {
  /* Fill node once for sure. */
  fill(x);

  /* Fill the node again under specific circumstances. */
  if (x->rank > t && x->rank % 2 == 0 && x->left != Null)
    fill(x);
}

void TarjanSoftHeap::fill(Node *x) {
  /* Ensure that left has the smaller key. */
  if (x->left->key > x->right->key) {
    swap(x->left, x->right);
  }

  /* Pull key and item set into x from x->left. */
  x->key = x->left->key;
  if (x->set == NULL) {
    x->set = x->left->set;
  } else {
    swap(x->set->next, x->left->set->next);
  }
  x->left->set = NULL;

  /* Fill the left if it isn't a leaf node, or delete it. */
  if (x->left->left == Null) {
    delete x->left;
    x->left = x->right;
    x->right = Null;
  } else {
    doubleEvenFill(x->left);
  }
}

/* Convert heap from findable to meldable order. */
Node *TarjanSoftHeap::rankSwap(Node *h) {
  Node *x = h->next;
  if (h->rank <= x->rank)
    return h;

  h->next = x->next;
  x->next = h;
  return x;
}

/* Convert heap from meldable to findable order. */
Node *TarjanSoftHeap::keySwap(Node *h) {
  Node *x = h->next;
  if (h->key <= x->key)
    return h;

  h->next = x->next;
  x->next = h;
  return x;
}

int TarjanSoftHeap::deleteMin() {
  int key;
  /* This shouldn't happen. */
  if (heap->set == NULL) {
    cout << "Min item set empty when deleting?" << endl;
    return -1;
  }

  Item *e = heap->set->next;
  /* If we're not emptying the item set, just fix item set. */
  if (e->next != e) {
    heap->set->next = e->next;
  } 
  /* If we are emptying the item set, we need to fill root and reorder tree list. */
  else {
    heap->set = NULL;
    int k = heap->rank;

    /* If heap is now empty, delete it. */
    if (heap->left == Null) {
      Node *remaining = heap->next;
      delete heap;
      heap = remaining;
    } 
    /* Otherwise, fill the heap. */
    else {
      doubleEvenFill(heap);
    }

    /* Restore findable order. */
    heap = reorder(heap, k);
  }

  /* Extract the key and delete its item container. */
  key = e->key;
  delete e;
  return key;
}

Node *TarjanSoftHeap::reorder(Node *h, int k) {
  /* Recurse if next rank is less than k. */
  if (h->next->rank < k) {
    h = rankSwap(h);
    h->next = reorder(h->next, k);
  }
  /* Convert to findable order before returning. */
  return keySwap(h);
}

void TarjanSoftHeap::insert(int key) {
  /* Create new item with key. */
  Item *e = new Item();
  e->key = key;

  /* Make e a singleton node and then insert using meldable insert. */
  heap = keySwap(meldableInsert(makeRoot(e), rankSwap(heap)));
}

Node *TarjanSoftHeap::meldableInsert(Node *x, Node *h) {
  /* If x->rank < h->rank, then we have x in the right place. */
  if (x->rank < h->rank) {
    x->next = keySwap(h);
    return x;
  } 
  /* Link x and h together, then insert into the rest of the heap. */
  else {
    return meldableInsert(link(x, h), rankSwap(h->next));
  }
}

Node *TarjanSoftHeap::makeRoot(Item *e) {
  /* Make new node with e. */
  Node *x = new Node();
  e->next = e;
  x->set = e;
  x->key = e->key;
  x->rank = 0;
  x->left = Null;
  x->right = Null;
  x->next = Null;
  return x;
}

Node *TarjanSoftHeap::link(Node *x, Node *y) {
  /* Make new node. */
  Node *z = new Node();
  z->set = NULL;

  /* Hoist z over x and y. */
  z->rank = x->rank + 1;
  z->left = x;
  z->right = y;

  /* Fill z. */
  doubleEvenFill(z);
  return z;
}

void TarjanSoftHeap::meld(Node *h) {
  /* Put both heaps in meldable order, then meld them. */
  heap = keySwap(meldableMeld(rankSwap(heap), rankSwap(h)));
}

Node *TarjanSoftHeap::meldableMeld(Node *h1, Node *h2) {
  /* Ensure h1 has smaller rank. */
  if (h1->rank > h2->rank) {
    swap(h1, h2);
  }

  /* If h2 is Null, then h1 is melded already. */
  if (h2 == Null)
    return h1;
  /* Remove smallest rank thing, insert it into meld of the rest. */
  else
    return meldableInsert(h1, meldableMeld(rankSwap(h1->next), h2));
}




















