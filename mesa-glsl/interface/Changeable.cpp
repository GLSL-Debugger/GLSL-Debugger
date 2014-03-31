/*
 * Changeable.cpp
 *
 *  Created on: 06.09.2013
 */

#include "Shader.h"
#include "AstScope.h"
#include "glsldb/utils/dbgprint.h"
#include <string.h>
#include <unordered_set>


void dumpShChangeable(ShChangeable *cgb)
{
	if (cgb) {
		dbgPrint(DBGLVL_INFO, "%i", cgb->id);
		for (int j = 0; j < cgb->numIndices; j++) {
			ShChangeableIndex *idx = cgb->indices[j];
			if (idx) {
				switch (idx->type) {
				case SH_CGB_ARRAY_DIRECT:
					dbgPrint(DBGLVL_INFO, "[%i]", idx->index);
					break;
				case SH_CGB_ARRAY_INDIRECT:
					dbgPrint(DBGLVL_INFO, "[(%i)]", idx->index);
					break;
				case SH_CGB_STRUCT:
					dbgPrint(DBGLVL_INFO, ".%i", idx->index);
					break;
				case SH_CGB_SWIZZLE:
					dbgPrint(DBGLVL_INFO, ",%i", idx->index);
					break;
				default:
					break;
				}
			}
		}
		dbgPrint(DBGLVL_INFO, " ");
	}
}

void dumpShChangeableList(ShChangeableList *cl)
{
	int i;

	if (!cl)
		return;
	dbgPrint(DBGLVL_INFO, "===> ");
	if (cl->numChangeables == 0) {
		dbgPrint(DBGLVL_INFO, "empty\n");
		return;
	}

	for (i = 0; i < cl->numChangeables; i++) {
		ShChangeable *cgb = cl->changeables[i];
		dumpShChangeable(cgb);
	}
	dbgPrint(DBGLVL_INFO, "\n");
}

static bool isEqualShChangeable(ShChangeable *a, ShChangeable *b)
{
	int i;

	if (a->id != b->id)
		return false;
	if (a->numIndices != b->numIndices)
		return false;

	for (i = 0; i < a->numIndices; i++) {
		if (a->indices[i]->type != b->indices[i]->type)
			return false;
		if (a->indices[i]->index != b->indices[i]->index)
			return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////
// Old functions
//

ShChangeable* createShChangeable(int id)
{
	dbgPrint(DBGLVL_INTERNAL_WARNING, "Creation of changeable out of shader mem.\n");
	return createShChangeableCtx(id, NULL);
}

ShChangeableIndex* createShChangeableIndex(ShChangeableType type, int index)
{
	dbgPrint(DBGLVL_INTERNAL_WARNING, "Creation of changeable index out of shader mem.\n");
	return createShChangeableIndexCtx(type, index, NULL);
}

void addShChangeable(ShChangeableList *cl, ShChangeable *c)
{
	dbgPrint(DBGLVL_INTERNAL_WARNING, "Realloc of changeables out of shader mem.\n");
	addShChangeableCtx(cl, c, NULL);
}

// FIXME: is this reasonable
ShChangeable * copyShChangeable(ShChangeable *c)
{
	dbgPrint(DBGLVL_INTERNAL_WARNING, "Copy of changeable out of shader mem.\n");
	return copyShChangeableCtx(c, NULL);
}

void copyShChangeableToList(ShChangeableList *cl, ShChangeable *c)
{
	dbgPrint(DBGLVL_INTERNAL_WARNING, "Copy changeable to list out of shader mem.\n");
	copyShChangeableToListCtx(cl, c, NULL);
}

void copyShChangeableList(ShChangeableList *clout, ShChangeableList *clin)
{
	int i, j;

	dbgPrint(DBGLVL_INTERNAL_WARNING, "Deprecated copyShChangeableList.\n");

	if (!clout || !clin)
		return;

	for (i = 0; i < clin->numChangeables; i++) {
		// copy only if not already in list
		bool alreadyInList = false;
		for (j = 0; j < clout->numChangeables; j++) {
			if (isEqualShChangeable(clout->changeables[j],
					clin->changeables[i])) {
				alreadyInList = true;
				break;
			}
		}
		if (!alreadyInList)
			copyShChangeableToList(clout, clin->changeables[i]);
	}
}

void addShIndexToChangeable(ShChangeable *c, ShChangeableIndex *idx)
{
	dbgPrint(DBGLVL_INTERNAL_WARNING, "Adding of ChangeableIndex out of shader mem.\n");
	addShIndexToChangeableCtx(c, idx, NULL);
}

void addShIndexToChangeableList(ShChangeableList *cl, int s,
		ShChangeableIndex *idx)
{
	dbgPrint(DBGLVL_INTERNAL_WARNING, "Adding of ChangeableIndex to list out of shader mem.\n");
	if (!cl)
		return;
	if (s < cl->numChangeables)
		addShIndexToChangeable(cl->changeables[s], idx);
}


////////////////////////////////////////////////////////////////////////
// Mem-safe functions
//

ShChangeable* createShChangeableCtx(int id, void* mem_ctx)
{
	ShChangeable *cgb = (ShChangeable*)rzalloc(mem_ctx, ShChangeable);
	cgb->id = id;
	return cgb;
}

ShChangeableIndex* createShChangeableIndexCtx(ShChangeableType type, long index, void* mem_ctx)
{
	ShChangeableIndex *idx = (ShChangeableIndex*)rzalloc(mem_ctx, ShChangeableIndex);
	idx->type = type;
	idx->index = index;
	return idx;
}

void addShChangeableCtx(ShChangeableList *cl, ShChangeable *c, void* mem_ctx)
{
	if (!cl || !c)
		return;

	cl->numChangeables++;
	cl->changeables = (ShChangeable**) reralloc_array_size(mem_ctx, cl->changeables,
			sizeof(ShChangeable*), cl->numChangeables);
	cl->changeables[cl->numChangeables - 1] = c;
}

ShChangeable * copyShChangeableCtx(ShChangeable *c, void* mem_ctx)
{
	int i;
	ShChangeable *copy;

	if (!c)
		return NULL;

	copy = createShChangeableCtx(c->id, mem_ctx);

	// add all indices
	for (i = 0; i < c->numIndices; i++) {
		copy->numIndices++;
		copy->indices = (ShChangeableIndex**)reralloc_array_size(mem_ctx, copy->indices,
				sizeof(ShChangeableIndex*), copy->numIndices);

		copy->indices[copy->numIndices - 1] = createShChangeableIndexCtx(
						c->indices[i]->type, c->indices[i]->index, mem_ctx);
	}

	return copy;
}

void copyShChangeableToListCtx(ShChangeableList *cl, ShChangeable *c, void* mem_ctx)
{
	ShChangeable *copy;
	if (!cl || !c)
		return;

	copy = copyShChangeableCtx(c, mem_ctx);
	addShChangeableCtx(cl, copy, mem_ctx);
}

void copyShChangeableListCtx(ShChangeableList *clout, exec_list *clin, void* mem_ctx)
{
	if (!clout || !clin)
		return;

	foreach_list(node, clin) {
		changeable_item* ch_item = (changeable_item*) node;
		bool alreadyInList = false;
		for (int j = 0; j < clout->numChangeables; j++) {
			if (isEqualShChangeable(clout->changeables[j], ch_item->changeable)) {
				alreadyInList = true;
				break;
			}
		}
		if (!alreadyInList)
			copyShChangeableToListCtx(clout, ch_item->changeable, mem_ctx);
	}
}

void copyAstChangeableList(exec_list *clout, exec_list *clin, exec_list* only, void* mem_ctx)
{
	if (!clout || !clin || clin->is_empty())
		return;

	std::unordered_set<int> permits;
	if (only)
		foreach_list(node, only)
			permits.emplace(((scope_item*)node)->id);


	// TODO: this algorithm is bad.
	foreach_list(node, clin){
		changeable_item* ch_item = (changeable_item*)node;
		if (only && permits.find(ch_item->id) == permits.end())
			continue;
		bool alreadyInList = false;
		foreach_list(onode, clout) {
			changeable_item* cho_item = (changeable_item*)onode;
			if (!isEqualShChangeable(ch_item->changeable, cho_item->changeable))
				continue;
			alreadyInList = true;
			break;
		}
		if (!alreadyInList)
			clout->push_tail(ch_item->clone(mem_ctx));
	}
}

void addShIndexToChangeableCtx(ShChangeable *c, ShChangeableIndex *idx, void* mem_ctx)
{
	if (!c)
		return;

	c->numIndices++;
	c->indices = (ShChangeableIndex**) reralloc_array_size(mem_ctx, c->indices,
			sizeof(ShChangeableIndex*), c->numIndices);
	c->indices[c->numIndices - 1] = idx;
}

void freeShChangeable(ShChangeable **c)
{
	if (c && *c) {
		int i;
		for (i = 0; i < (*c)->numIndices; i++)
			ralloc_free((*c)->indices[i]);
		ralloc_free((*c)->indices);
		ralloc_free(*c);
		*c = NULL;
	}
}

