/*
 * IRStack.h
 *
 *  Created on: 13.09.2013
 */

#ifndef __IRSTACK_H_
#define __IRSTACK_H_

struct ir_instruction;


class IRStack
{
public:
	IRStack( );
	~IRStack( );

	void push(ir_instruction* n);
	void pop(void);
	ir_instruction* top(void);

	int empty(void);

private:
    int n;
    ir_instruction **s;
};

#endif /* __IRSTACK_H_ */
