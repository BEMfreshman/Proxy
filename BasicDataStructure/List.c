#include "List.h"
#include <string.h>


bool list_getnext(list lt, node* nd) {
    if (lt->length == 0) {
        nd = NULL;
        return false;
    }


    if (nd == NULL) {
        // first enter this function
        nd = lt->first;
        return true;
    }
    else {
        nd = nd->next;
        return nd == NULL ? false : true;
    }
}



list createList()
{
    list lt= malloc(sizeof(list_));
    lt->length = 0;
    lt->first = NULL;
    lt->last = NULL;
    lt->getnext = list_getnext;
    return lt;
}

void freelist(list lt) {
    for (int i = 0; i < lt->length; i++) {
        node* nd = lt->first;
        lt->first = nd->next;
        
        nd->prev = NULL;
        nd->next = NULL;
        free(nd);
    }
}

void reset(list lt) {
    list nlt = createList();
    freelist(lt);
    lt = nlt;
}


void appendnode(list lt,void* dt,size_t sizeofdt)
{
    node* nd = malloc(sizeof(node));
    nd->sz = sizeofdt;
    
    nd->data = dt;

    if (lt->length == 0) {
        lt->first = nd;
        lt->last = nd;
        lt->length++;
    }
    else {
        lt->last->next = nd;
        nd->prev = lt->last;
        nd->next = NULL;
        lt->last = nd;
        lt->length++;
    }
}

void addheadnode(list lt, void* dt, size_t sizeofdt)
{
    node* nd = malloc(sizeof(node));
    nd->sz = sizeofdt;
    
    nd->data = dt;

    if (lt->length == 0) {
        lt->first = nd;
        lt->last = nd;
        lt->length++;
    }
    else {
        lt->first->prev = nd;
        nd->prev = NULL;
        nd->next = lt->first;
        lt->first = nd;
        lt->length++;
    }
}

node* getat(list lt,size_t n, int newalloc)
{
    if (n >= lt->length) return NULL;
    node* nd = lt->first;
    
    if (n != 0) {
        for (size_t i = 1; i < n; i++) {
            nd = nd->next;
        }
    }

    //now re is pointing to n-th ele in lt


    if (newalloc == 0) {
        return nd;
    }
    else {
        node* re = malloc(nd->sz);
        memcpy(re, nd, nd->sz);
        return re;
    }
}

node* popat2(list lt, node* nd) {
    node* tmp = NULL;
    size_t counter = 0;
    while (lt->getnext(lt, tmp)) {
        if (tmp != nd) {
            counter++;
        }
        else {
            return popat(lt, counter);
        }
    }

    return NULL;
}

node* popat(list lt, size_t n) {
    node* nd = getat(lt,n,0);
    if (nd == NULL) return;

    if (lt->length == 1) {
        lt->first = NULL;
        lt->last = NULL;
    }

    if (n == 0) {
        //pop first
        lt->first = lt->first->next;
        nd->next->prev = NULL;
    }
    else if (n == lt->length - 1) {
        //pop last
        lt->last = lt->last->prev;
        nd->prev->next = NULL;
    }
    else {
        nd->prev->next = nd->next;
        nd->next->prev = nd->prev;
    }

    nd->next = NULL;
    nd->prev = NULL;
    
    lt->length--;

    return nd;
}

