#include "SoftHeap.h"
#include <limits>
#include <iostream>
#include <algorithm>
#include <string>
using namespace std;

SoftHeap::SoftHeap() {
  header = new Head();
  header->queue = NULL;
  header->prev = NULL;
  header->suffixMin = NULL;
  header->rank = 0;

  tail = new Head();
  tail->queue = NULL;
  tail->next = NULL;
  tail->suffixMin = NULL;
  tail->rank = numeric_limits<int>::max();

  header->next = tail;
  tail->prev = header;
  r = 4;
}

SoftHeap::~SoftHeap() { }

void SoftHeap::insert(int key) {
  /* Create new item. */
  ILCell *l = new ILCell();
  l->key = key;
  l->next = NULL;

  /* Create new node and insert single item. */
  Node *q = new Node();
  q->next = q->child = NULL;
  q->rank = 0;
  q->ckey = key;
  q->il = l;
  q->ilTail = l;

  /* Meld new node into soft heap. */
  meld(q);
}

void SoftHeap::meld(Node *q) {
  Head *h, *prevHead, *toHead = header->next;
  Node *top, *bottom;

  /* Find right place for soft queue. */
  while (q->rank > toHead->rank)
    toHead = toHead->next;
  prevHead = toHead->prev;

  /* Merge soft queues until carry propagation is done (like in binomial queue). */
  while (q->rank == toHead->rank) {

    /* Find which queue to hoist over the other. */
    if (toHead->queue->ckey > q->ckey) {
      top = q;
      bottom = toHead->queue;
    } else {
      top = toHead->queue;
      bottom = q;
    }

    /* We must make a new node in left-path binarized tree. */
    q = new Node();
    q->ckey = top->ckey;
    q->rank = top->rank + 1;
    q->child = bottom;
    q->next = top;
    q->il = top->il;
    q->ilTail = top->ilTail;

    /* Potentially propagate further. */
    toHead = toHead->next;
  }

  /* Rewire header list with new soft queue q. */
  if (prevHead == toHead->prev) {
    h = new Head();
    h->suffixMin = NULL;
  } else {
    h = prevHead->next;
  }

  /* Insert h into header list. */
  h->queue = q;
  h->rank = q->rank;
  h->prev = prevHead;
  h->next = toHead;
  prevHead->next = h;
  toHead->prev = h;

  /* Go back through and fix the suffix minima. */
  fixMinList(h);
}

int SoftHeap::deleteMin() {
  Node *temp;
  int min, childCount;
  Head *h = header->next->suffixMin;

  /* Fill the root if it is empty. */
  while (h->queue->il == NULL) {
    temp = h->queue;
    childCount = 0;

    /* Count the children of the root to check if rank invariant holds. */
    while (temp->next != NULL) {
      temp = temp->next;
      childCount++;
    }

    /* Ensure the rank invariant holds. */
    if (childCount < h->rank / 2) {
      h->prev->next = h->next;
      h->next->prev = h->prev;
      fixMinList(h->prev);

      /* Dismantle and meld to regain rank invariant. */
      temp = h->queue;
      while (temp->next != NULL) {
        meld(temp->child);
        temp = temp->next;
      }
    } 
    /* If rank invariant holds, we refill by calling sift. */
    else {
      h->queue = sift(h->queue, "");
      if (h->queue->ckey == numeric_limits<int>::max()) {
        h->prev->next = h->next;
        h->next->prev = h->prev;
        h = h->prev;
      }
      fixMinList(h);
    }
    h = header->next->suffixMin;
  }

  /* Find and extract the min. */
  min = h->queue->il->key;
  h->queue->il = h->queue->il->next;
  if (h->queue->il == NULL)
    h->queue->ilTail = NULL;
  return min;
}

void SoftHeap::fixMinList(Head *h) {
  Head *tempMin;
  if (h->next == tail) tempMin = h;
  else tempMin = h->next->suffixMin;
  while (h != header) {
    if (h->queue->ckey < tempMin->queue->ckey)
      tempMin = h;
    h->suffixMin = tempMin;
    h = h->prev;
  }
}

Node *SoftHeap::sift(Node *v, string spaces) {
  Node *temp;
  v->il = NULL;
  v->ilTail = NULL;

  /* If v is a leaf, set its key to infinity and return it. */
  if (v->next == NULL && v->child == NULL) {
    v->ckey = numeric_limits<int>::max();
    return v;
  }
  
  /* First recursive sift. */
  v->next = sift(v->next, "\t"+spaces);

  /* "Perform a rotation by exchanging children v->next and v->child" */
  if (v->next->ckey > v->child->ckey) {
    temp = v->child;
    v->child = v->next;
    v->next = temp;
  }

  /* Pass the item list up from v->next. */
  v->il = v->next->il;
  v->ilTail = v->next->ilTail;
  v->ckey = v->next->ckey;

  /* Potential second recursive sift. */
  if (v->rank > r && (v->rank % 2 == 1 || v->child->rank < v->rank - 1)) {
    v->next = sift(v->next, "\t"+spaces);

    /* Rotate to preserve heap ordering. */
    if (v->next->ckey > v->child->ckey) {
      temp = v->child;
      v->child = v->next;
      v->next = temp;
    }
    /* Concatenate the item list at v->next with v. */
    if (v->next->ckey != numeric_limits<int>::max() && v->next->il != NULL) {
      v->next->ilTail->next = v->il;
      v->il = v->next->il;
      if (v->ilTail == NULL)
        v->ilTail = v->next->ilTail;
      v->ckey = v->next->ckey;
    }
  }

  /* Clean up nodes with infinite ckeys. */
  if (v->child->ckey == numeric_limits<int>::max()) {
    if (v->next->ckey == numeric_limits<int>::max()) {
      v->child = NULL;
      v->next = NULL;
    } else {
      v->child = v->next->child;
      v->next = v->next->next;
    }
  }

  return v;
}

void SoftHeap::printQueue(Node *n, int depth, string spaces) {
  if (n->child != NULL) {
    printQueue(n->child, depth + 1, spaces);
  }
  cout << spaces;
  for (int i = 0; i < depth; i++)
    cout << "\t";
  cout << "(" << n->ckey << "){";
  if (n->il != NULL) {
    ILCell *curr = n->il;
    while (curr != n->ilTail) {
      cout << curr->key << ", ";
      curr = curr->next;
    }
    cout << curr->key;
  }
  cout << "}" << endl;

  if (n->next != NULL) {
    printQueue(n->next, depth + 1, spaces);
  }
}

void SoftHeap::printQueueAsBinomialTree(Node *n, int depth) {
  if (n->child != NULL) {
    printQueueAsBinomialTree(n->child, depth + 1);
  }

  if (n->next == NULL) {
    for (int i = 0; i < depth; i++)
      cout << "\t";
    cout << "(" << n->ckey << "){";
    if (n->il != NULL) {
      ILCell *curr = n->il;
      while (curr != n->ilTail) {
        cout << curr->key << ", ";
        curr = curr->next;
      }
      cout << curr->key;
    }
    cout << "}" << endl;
  }

  if (n->next != NULL) {
    printQueueAsBinomialTree(n->next, depth);
  }
}

void SoftHeap::createBinomialTree(Node *n, BinTree *parent) {
  if (n->next == NULL)
    return;

  createBinomialTree(n->next, parent);

  // As in Chazelle's paper, either next and child are both NULL or neither.
  // Thus n->child can't be NULL here.
  
  BinTree *child = new BinTree();  
  child->ckey = n->child->ckey;
  child->il = n->child->il;
  child->ilTail = n->child->ilTail; 
  parent->children.push_back(child);

  createBinomialTree(n->child, child);
}

string SoftHeap::createBinTreeStrings(vector<string>& levels, int depth, 
        string baseSpaces, string secSpaces, BinTree *node) {
  if (node == NULL) return secSpaces;

  levels[depth - 1] += "(" + to_string(node->ckey) + "){";
  if (node->il != NULL) {
    ILCell *curr = node->il;
    while (curr != node->ilTail) {
      levels[depth - 1] += to_string(curr->key) + ", ";
      curr = curr->next;
    }
    levels[depth - 1] += to_string(curr->key);
  }
  levels[depth - 1] += "}";
  
  if (node->children.empty())
    return secSpaces;

  for (size_t i = 0; i < node->children.size(); i++) {
    if (i == 0 && (size_t)depth < levels.size() && depth > 1) {
      int prevLength = levels[depth - 1].rfind('(');
      int numSpaces = prevLength - levels[depth].length();
      if (numSpaces >= 0) {
          string tmp(numSpaces, ' ');
          levels[depth] += tmp;
      }
    }
    if (i != 0 && (size_t)depth < levels.size()) {
        size_t maxLen = levels[depth].length();
        for (int i = levels.size() - 1; i >= depth; i--) {
            if (levels[i].length() > maxLen)
                maxLen = levels[i].length();
        }
        int numSpaces = maxLen - levels[depth].length() + 2;
        string spaces(numSpaces, ' ');
        levels[depth] += (levels[depth] == "") ? baseSpaces : spaces;
    }
    secSpaces = createBinTreeStrings(levels, depth + 1, 
            string(levels[depth - 1].length() - 1, ' '), secSpaces + " ", 
            node->children[i]);
  }
  return secSpaces;
}

void SoftHeap::print(string spaces) {
  int queueNum = 0;
  Head *curr = header->next;
  while (curr != NULL && curr != tail) {
    cout << spaces << "Queue " << queueNum << " (rank " << curr->rank << "):" << endl;
    if (curr->queue != NULL) 
      printQueue(curr->queue, 0, spaces);
    queueNum++;
    curr = curr->next;
  }
  cout << spaces << "--------------------------------------------------------------" << endl;
}

void SoftHeap::printAsBinomialTrees(string spaces) {
  int treeNum = 0;
  Head *curr = header->next;
  while (curr != NULL && curr != tail) {
    cout << spaces << "Binomial Tree " << treeNum << " (rank " << curr->rank << "):" << endl;
    if (curr->queue != NULL) {
      BinTree *root = new BinTree();
      root->ckey = curr->queue->ckey;
      root->il = curr->queue->il;
      root->ilTail = curr->queue->ilTail;
      createBinomialTree(curr->queue, root);
      vector<string> levels(curr->rank + 1);
      fill(levels.begin(), levels.end(), "");
      createBinTreeStrings(levels, 1, "", "", root);
      for (size_t i = 0; i < levels.size(); i++) {
        cout << spaces << levels[i] << endl;
      }
    }
    treeNum++;
    curr = curr->next;
  }
  cout << spaces << "--------------------------------------------------------------" << endl;
}


