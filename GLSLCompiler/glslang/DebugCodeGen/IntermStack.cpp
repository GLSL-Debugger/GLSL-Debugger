#include "IntermStack.h"
#include <stdlib.h>
#include <stdio.h>

TIntermStack::TIntermStack()
{
    n = 0;
    s = NULL;
}

TIntermStack::~TIntermStack()
{
    free(s);
}

void TIntermStack::push(TIntermNode *node)
{
    n++;
    s = (TIntermNode**) realloc(s, n * sizeof(TIntermNode*));
    s[n-1] = node;
}

void TIntermStack::pop(void)
{
    n--;
    s = (TIntermNode**) realloc(s, n * sizeof(TIntermNode*));
}

TIntermNode* TIntermStack::top(void)
{
    if (n==0) {
        return NULL;
    } else {
        return s[n-1];
    }
}

int TIntermStack::empty(void)
{
    return (n==0);
}

