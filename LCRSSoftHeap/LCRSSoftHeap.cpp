#include "LCRSSoftHeap.h"
#include <limits>
#include <iostream>

using namespace std;

LCRSSoftHeap::LCRSSoftHeap() {
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

LCRSSoftHeap::~LCRSSoftHeap() { }

void LCRSSoftHeap::insert(int key) {
  /* Create new item. */
  ILCell *l = new ILCell();
  l->key = key;
  l->next = NULL;

  /* Create new node and insert single item. */
  Node *q = new Node();
  q->lc = q->rs = NULL;
  q->rank = 0;
  q->ckey = key;
  q->il = l;
  q->ilTail = l;

  /* Meld new node into soft heap. */
  meld(q);
}

void LCRSSoftHeap::meld(Node *q) {
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
    bottom->rs = top->lc;
    top->lc = bottom;
    q = top;
    q->rank += 1;

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

int LCRSSoftHeap::deleteMin() {
  Node *temp;
  int min, childCount;
  Head *h = header->next->suffixMin;

  /* Fill the root if it is empty. */
  while (h->queue->il == NULL) {
    temp = h->queue->lc;
    childCount = 0;

    /* Count the children of the root to check if rank invariant holds. */
    while (temp != NULL) {
      childCount++;
      temp = temp->rs;
    }

    /* Ensure the rank invariant holds. */
    if (childCount < h->rank / 2) {
      h->prev->next = h->next;
      h->next->prev = h->prev;
      fixMinList(h->prev);

      /* Dismantle and meld to regain rank invariant. */
      temp = h->queue->lc;
      while (temp != NULL) {
        Node *next = temp->rs;
        meld(temp);
        temp = next;
      }
    } 
    /* If rank invariant holds, we refill by calling sift. */
    else {
      h->queue = sift(h->queue);
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

void LCRSSoftHeap::fixMinList(Head *h) {
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

Node *LCRSSoftHeap::sift(Node *v) {
  Node *temp;
  v->il = NULL;
  v->ilTail = NULL;

  /* If v is a leaf, set its key to infinity and return it. */
  if (v->lc == NULL) {
    v->ckey = numeric_limits<int>::max();
    return v;
  }
  
  /* Find minimum key child. */
  Node *prev = NULL;
  Node *min = v->lc;
  temp = min;
  while (temp->rs != NULL) {
    Node *tempPrev = temp;
    temp = temp->rs;
    if (min->ckey > temp->ckey) {
      min = temp;
      prev = tempPrev;
    }
  }

  /* Rotate with minimum key child. */
  if (min == v->lc) {
    v->lc = min->rs;
    min->rs = v->rs;
    v->rs = min->lc;
    min->lc = v;
  } else {
    prev->rs = v;
    temp = v->rs;
    v->rs = min->lc;
    min->lc = v->lc;
    v->lc = min->rs;
    min->rs = temp;
  }
  swap(min->rank, v->rank);

  /* Sift on v. */
  if (min->lc == v) {
    min->lc = sift(v);
    v = min->lc;
  } else {
    prev->rs = sift(v);
    v = prev->rs;
  }

  /* If there was nothing to sift up, trim v out. */
  if (v->ckey == numeric_limits<int>::max()) {
    if (prev) {
      prev->rs = v->rs;
    } else {
      min->lc = v->rs;
    }
  }

  v = min;

  /* Potential second recursive sift. */
  if (v->rank > r && v->lc != NULL && (v->rank % 2 == 1 || v->lc->rank < v->rank - 1)) {
    ILCell *list = v->il;
    ILCell *listTail = v->ilTail;
    int maxKey = v->ckey;
    v->il = v->ilTail = NULL;
    
    /* Find minimum key child. */
    Node *prev = NULL;
    Node *min = v->lc;
    temp = min;
    while (temp->rs != NULL) {
      Node *tempPrev = temp;
      temp = temp->rs;
      if (min->ckey > temp->ckey) {
        min = temp;
        prev = tempPrev;
      }
    }

    /* Rotate with minimum key child. */
    if (min == v->lc) {
      v->lc = min->rs;
      min->rs = v->rs;
      v->rs = min->lc;
      min->lc = v;
    } else {
      prev->rs = v;
      temp = v->rs;
      v->rs = min->lc;
      min->lc = v->lc;
      v->lc = min->rs;
      min->rs = temp;
    }
    swap(min->rank, v->rank);

    /* Sift on v. */
    if (min->lc == v) {
      min->lc = sift(v);
      v = min->lc;
    } else {
      prev->rs = sift(v);
      v = prev->rs;
    }

    /* If there was nothing to sift up, trim v out. */
    if (v->ckey == numeric_limits<int>::max()) {
      if (prev) {
        prev->rs = v->rs;
      } else {
        min->lc = v->rs;
      }
    }

    v = min;

    /* Concatenate the item list at v->next with v. */
    if (v->ckey != numeric_limits<int>::max() && v->il != NULL) {
      listTail->next = v->il;
      v->il = list;
      if (v->ilTail == NULL)
        v->ilTail = listTail;
      v->ckey = max(v->ckey, maxKey);
    } else {
      v->il = list;
      v->ilTail = listTail;
      v->ckey = maxKey;
    }
  }
  return v;
}


