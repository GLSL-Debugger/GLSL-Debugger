#ifndef __INTERMSTACK_H
#define __INTERMSTACK_H

#include <Include/intermediate.h>

class TIntermStack {
public:
    TIntermStack();
    ~TIntermStack();

    void push(TIntermNode* n);
    void pop(void);
    TIntermNode* top(void);

    int empty(void);

private:
    int n;
    TIntermNode **s;
};

#endif

