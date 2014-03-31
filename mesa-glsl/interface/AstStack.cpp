/*
 * IRStack.cpp
 *
 *  Created on: 13.09.2013
 */

#include "AstStack.h"
#include "mesa/glsl/ast.h"
#include <stdlib.h>


AstStack::AstStack()
{
	n = 0;
	s = NULL;
	iter = 0;
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

ast_node* AstStack::base(void)
{
	if (empty())
		return NULL;
	return s[0];
}

ast_node* AstStack::top(void)
{
	if (empty())
		return NULL;
	return s[n - 1];
}

bool AstStack::empty(void)
{
	return !(n > 0);
}

void AstStack::clear()
{
	while (!empty())
		pop();
}

void AstStack::copy(AstStack* to)
{
	for (int i = 0; i < n; ++i)
		 to->push(s[i]);
}

ast_node* AstStack::next()
{
	if (iter >= n)
		return NULL;
	return s[iter++];
}

ast_node* AstStack::perv()
{
	if (iter < 0)
		return NULL;
	return s[iter--];
}

ast_node* AstStack::head()
{
	iter = n - 1;
	return top();
}

ast_node* AstStack::back()
{
	iter = 0;
	return base();
}
