/*
 * IRStack.cpp
 *
 *  Created on: 13.09.2013
 */

#include "IRStack.h"
#include "glsl/ir.h"
#include <stdlib.h>

IRStack::IRStack()
{
	n = 0;
	s = NULL;
}

IRStack::~IRStack()
{
	free(s);
}

void IRStack::push(ir_instruction* node)
{
	n++;
	s = (ir_instruction**) realloc(s, n * sizeof(ir_instruction*));
	s[n - 1] = node;
}

void IRStack::pop(void)
{
	n--;
	s = (ir_instruction**) realloc(s, n * sizeof(ir_instruction*));
}

ir_instruction* IRStack::top(void)
{
	if (n == 0) {
		return NULL;
	} else {
		return s[n - 1];
	}
}

int IRStack::empty(void)
{
	return (n == 0);
}

