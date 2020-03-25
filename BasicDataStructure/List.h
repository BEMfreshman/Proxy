#pragma once

#include <stdlib.h>
#include <stdbool.h>

typedef struct node_ node;
typedef struct _list_ list_;
typedef list_* list;

struct node_ {
    void* data;
 //   int deepcopy;
    size_t sz;

    struct node_* prev;
    struct node_* next;

};

extern bool list_getnext(list lt, node* nd);

struct _list_ {
    size_t length;
    node* first;
    node* last;

    bool (*getnext)(list lt, node* nd);
};


list createList();
void freelist(list lt);
void reset(list lt);

void appendnode(list lt,void* data,size_t sizeofdt);
void addheadnode(list lt,void* data,size_t sizeofdt);
node* getat(list lt,size_t n,int newalloc);
node* popat(list lt,size_t n);
node* popat2(list lt, node* nd);
