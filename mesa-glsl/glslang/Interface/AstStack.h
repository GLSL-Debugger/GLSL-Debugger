/*
 * AstStack.h
 *
 *  Created on: 13.09.2013
 */

#ifndef AST_STACK_H_
#define AST_STACK_H_

class ast_node;

class AstStack {
public:
	AstStack();
	~AstStack();

	void push(ast_node* n);
	void pop(void);
	ast_node* base(void);
	ast_node* top(void);

	bool empty(void);
	void clear();

	void copy(AstStack*);

	// Iteration methods
	ast_node* next();
	ast_node* perv();
	ast_node* head();
	ast_node* back();

private:
	int iter;
	int n;
	ast_node **s;
};

#define foreach_stack(__node, __stack) \
	for (ast_node* __node = __stack->head(); __node != NULL; __node = __stack->perv())

#define foreach_stack_reverse(__node, __stack) \
	for (ast_node* __node = __stack->back(); __node != NULL; __node = __stack->next())

#endif /* AST_STACK_H_ */
