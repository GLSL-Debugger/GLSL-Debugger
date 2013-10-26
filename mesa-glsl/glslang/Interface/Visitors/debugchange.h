/*
 * debugchange.h
 *
 *  Created on: 20.10.2013
 */

#ifndef DEBUGCHANGE_H_
#define DEBUGCHANGE_H_

#include "traverse.h"
#include <set>

class ir_debugchange_traverser_visitor : public ir_traverse_visitor {
public:
	ir_debugchange_traverser_visitor()
	{
	}

	virtual ~ir_debugchange_traverser_visitor()
	{
	}

	// Subclasses must implement this
	virtual bool visitIr(ir_variable *ir);
	virtual bool visitIr(ir_function_signature *ir);
	virtual bool visitIr(ir_function *ir);
	virtual bool visitIr(ir_expression *ir);
	virtual bool visitIr(ir_texture *ir);
	virtual bool visitIr(ir_swizzle *ir);
	virtual bool visitIr(ir_dereference_variable *ir);
	virtual bool visitIr(ir_dereference_array *ir);
	virtual bool visitIr(ir_dereference_record *ir);
	virtual bool visitIr(ir_assignment *ir);
	virtual bool visitIr(ir_constant *ir);
	virtual bool visitIr(ir_call *ir);
	virtual bool visitIr(ir_return *ir);
	virtual bool visitIr(ir_discard *ir);
	virtual bool visitIr(ir_if *ir);
	virtual bool visitIr(ir_loop *ir);
	virtual bool visitIr(ir_loop_jump *ir);

    bool isActive(void) { return active; }
    // active:  all coming symboles are being changed
    void activate(void) { active = true; }
    // passive: coming symboles act as input and are not changed
    void deactivate(void) { active = false; }

//    ir_instruction *root;
//    struct gl_shader* shader;
private:
    std::set<ir_function_signature*> parsed_signatures;
    bool active;
};


#endif /* DEBUGCHANGE_H_ */
