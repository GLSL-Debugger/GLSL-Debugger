/*
 * IRStack.cpp
 *
 *  Created on: 13.09.2013
 */

#include "AstStack.h"
#include "glsl/ast.h"
#include <stdlib.h>


AstStack::AstStack()
{
	n = 0;
	s = NULL;
}

AstStack::~AstStack()
{
	free(s);
}

void AstStack::push(ast_node* node)
{
	n++;
	s = (ast_node**) realloc(s, n * sizeof(ast_node*));
	s[n - 1] = node;
}

void AstStack::pop(void)
{
	if (empty())
		return;

	n--;
	s = (ast_node**) realloc(s, n * sizeof(ast_node*));
}

ast_node* AstStack::top(void)
{
	if (n == 0) {
		return NULL;
	} else {
		return s[n - 1];
	}
}

int AstStack::empty(void)
{
	return (n == 0);
}

void AstStack::clear()
{
	while (!empty())
		pop();
}
