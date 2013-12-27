/*
 * Shader.h
 *
 *  Created on: 20.09.2013
 */

#ifndef MESA_INTERFACE_SHADER_H_
#define MESA_INTERFACE_SHADER_H_

#include "ir.h"

struct exec_list;

bool dbg_state_not_match( exec_list*, enum ir_dbg_state );
bool dbg_state_not_match( ir_dummy*, enum ir_dbg_state );


#endif /* MESA_INTERFACE_SHADER_H_ */
