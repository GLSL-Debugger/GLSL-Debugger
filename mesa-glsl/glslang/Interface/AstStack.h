/*
 * IRStack.h
 *
 *  Created on: 13.09.2013
 */

#ifndef __IRSTACK_H_
#define __IRSTACK_H_

class ast_node;

class AstStack {
public:
	AstStack();
	~AstStack();

	void push(ast_node* n);
	void pop(void);
	ast_node* top(void);

	int empty(void);
	void clear();

private:
	int n;
	ast_node **s;
};

#endif /* __IRSTACK_H_ */
