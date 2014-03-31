/*
 * Format.h
 *
 *  Created on: 08.03.2014.
 */
#pragma once
#ifndef TEST_FORMAT_H_
#define TEST_FORMAT_H_

#include "interface/Shader.h"
#include <string>
#include <sstream>


std::string formatChangeable(ShChangeable *cgb)
{
	if (!cgb)
		return "";

	std::stringstream ss;
	ss << cgb->id << "{";
	for (int j = 0; j < cgb->numIndices; j++) {
		ShChangeableIndex *idx = cgb->indices[j];
		if (j)
			ss << " ";
		if (idx) {
			switch (idx->type) {
			case SH_CGB_ARRAY_DIRECT:
				ss << "[" << idx->index <<"]";
				break;
			case SH_CGB_ARRAY_INDIRECT:
				ss << "[(" << idx->index <<")]";
				break;
			case SH_CGB_STRUCT:
				ss << "." << idx->index;
				break;
			case SH_CGB_SWIZZLE:
				ss << "," << idx->index;
				break;
			default:
				break;
			}
		}
	}
	ss << "}";
	return ss.str();
}

std::string formatVariable(int id, ShVariable* var = NULL)
{
	if (!var)
		var = findShVariable(id);
	const char* name = var ? var->name : "undefined";
	std::stringstream ss;
	ss << "<" << id << "," << name << ">";
	return ss.str();
}

const char* DBG_STATUSES[4] = {
	"DBG_RS_STATUS_UNSET", "DBG_RS_STATUS_ERROR", "DBG_RS_STATUS_OK", "DBG_RS_STATUS_FINISHED"
};

const char* DBG_POSITIONS[17] = {
	"DBG_RS_POSITION_UNSET", "DBG_RS_POSITION_DECLARATION",
	"DBG_RS_POSITION_ASSIGMENT", "DBG_RS_POSITION_FUNCTION_CALL",
	"DBG_RS_POSITION_UNARY", "DBG_RS_POSITION_SELECTION_IF",
	"DBG_RS_POSITION_SELECTION_IF_ELSE", "DBG_RS_POSITION_SELECTION_IF_CHOOSE",
	"DBG_RS_POSITION_SELECTION_IF_ELSE_CHOOSE", "DBG_RS_POSITION_SWITCH",
    "DBG_RS_POSITION_SWITCH_CHOOSE", "DBG_RS_POSITION_BRANCH",
	"DBG_RS_POSITION_LOOP_FOR", "DBG_RS_POSITION_LOOP_WHILE",
	"DBG_RS_POSITION_LOOP_DO", "DBG_RS_POSITION_LOOP_CHOOSE",
	"DBG_RS_POSITION_DUMMY"
};

#endif /* TEST_FORMAT_H_ */
