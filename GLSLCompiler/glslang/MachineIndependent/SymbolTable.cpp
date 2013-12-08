//
//Copyright (C) 2002-2005  3Dlabs Inc. Ltd.
//All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions
//are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//    Neither the name of 3Dlabs Inc. Ltd. nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
//FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//POSSIBILITY OF SUCH DAMAGE.
//

//
// Symbol table for parsing.  Most functionaliy and main ideas
// are documented in the header file.
//

#include "SymbolTable.h"

//
// TType helper function needs a place to live.
//

//
// Recursively generate mangled names.
//
void TType::buildMangledName(TString& mangledName)
{
	if (isMatrix()) {
		mangledName += 'm';
	} else if (isVector()) {
		mangledName += 'v';
	}

	switch (type) {
	case EbtFloat:
		mangledName += 'f';
		break;
	case EbtInt:
		mangledName += 'i';
		break;
	case EbtUInt:
		mangledName += 'u';
		break;
	case EbtBool:
		mangledName += 'b';
		break;
	case EbtSampler1D:
		mangledName += "s1";
		break;
	case EbtISampler1D:
		mangledName += "sI1";
		break;  // EXT_gpu_shader4
	case EbtUSampler1D:
		mangledName += "sU1";
		break;  // EXT_gpu_shader4
	case EbtSampler2D:
		mangledName += "s2";
		break;
	case EbtISampler2D:
		mangledName += "sI2";
		break;  // EXT_gpu_shader4
	case EbtUSampler2D:
		mangledName += "sU2";
		break;  // EXT_gpu_shader4
	case EbtSampler3D:
		mangledName += "s3";
		break;
	case EbtISampler3D:
		mangledName += "sI3";
		break;  // EXT_gpu_shader4
	case EbtUSampler3D:
		mangledName += "sU3";
		break;  // EXT_gpu_shader4
	case EbtSamplerCube:
		mangledName += "sC";
		break;
	case EbtISamplerCube:
		mangledName += "sIC";
		break;  // EXT_gpu_shader4
	case EbtUSamplerCube:
		mangledName += "sUC";
		break;  // EXT_gpu_shader4
	case EbtSampler1DShadow:
		mangledName += "sS1";
		break;
	case EbtSampler2DShadow:
		mangledName += "sS2";
		break;
	case EbtSampler2DRect:
		mangledName += "sR2";
		break;  // ARB_texture_rectangle
	case EbtISampler2DRect:
		mangledName += "sIR2";
		break;  // EXT_gpu_shader4
	case EbtUSampler2DRect:
		mangledName += "sUR2";
		break;  // EXT_gpu_shader4
	case EbtSampler2DRectShadow:
		mangledName += "sSR2";
		break;  // ARB_texture_rectangle
	case EbtSampler1DArray:
		mangledName += "sA1";
		break;  // EXT_gpu_shader4
	case EbtISampler1DArray:
		mangledName += "sIA1";
		break;  // EXT_gpu_shader4
	case EbtUSampler1DArray:
		mangledName += "sUA1";
		break;  // EXT_gpu_shader4
	case EbtSampler2DArray:
		mangledName += "sA2";
		break;  // EXT_gpu_shader4
	case EbtISampler2DArray:
		mangledName += "sIA2";
		break;  // EXT_gpu_shader4
	case EbtUSampler2DArray:
		mangledName += "sUA2";
		break;  // EXT_gpu_shader4
	case EbtSamplerBuffer:
		mangledName += "sB";
		break;  // EXT_gpu_shader4
	case EbtISamplerBuffer:
		mangledName += "sIB";
		break;  // EXT_gpu_shader4
	case EbtUSamplerBuffer:
		mangledName += "sUB";
		break;  // EXT_gpu_shader4
	case EbtSampler1DArrayShadow:
		mangledName += "sAS1";
		break;  // EXT_gpu_shader4
	case EbtSampler2DArrayShadow:
		mangledName += "sAS2";
		break;  // EXT_gpu_shader4
	case EbtSamplerCubeShadow:
		mangledName += "sCS";
		break;  // EXT_gpu_shader4
	case EbtStruct:
		mangledName += "struct-";
		if (typeName) {
			mangledName += *typeName;
		}

		// support MSVC++6.0
		{
			for (unsigned int i = 0; i < structure->size(); ++i) {
				mangledName += '-';
				(*structure)[i].type->buildMangledName(mangledName);
			}
		}
	default:
		break;
	}
	if (isMatrix()) {
		mangledName += static_cast<char>('0' + getMatrixSize(0));
		mangledName += 'x';
		mangledName += static_cast<char>('0' + getMatrixSize(1));
	} else {
		mangledName += static_cast<char>('0' + getNominalSize());
	}
	if (isArray()) {
		char buf[100];
		sprintf(buf, "%d", arraySize[0]);
		mangledName += '[';
		mangledName += buf;
		mangledName += ']';
	}
}

int TType::getStructSize() const
{
	if (!getStruct()) {
		assert(false && "Not a struct");
		return 0;
	}

	if (structureSize == 0)
		for (TTypeList::iterator tl = getStruct()->begin();
				tl != getStruct()->end(); tl++)
			structureSize += ((*tl).type)->getObjectSize();

	return structureSize;
}

//
// Dump functions.
//

void TVariable::dump(TInfoSink& infoSink) const
{
	infoSink.debug << getName().c_str() << ": " << type.getQualifierString()
			<< " " << type.getBasicString();
	if (type.isArray()) {
		infoSink.debug << "[0]";
	}
	infoSink.debug << "\n";
}

void TFunction::dump(TInfoSink &infoSink) const
{
	infoSink.debug << getName().c_str() << ": " << returnType.getBasicString()
			<< " " << getMangledName().c_str() << "\n";
}

void TSymbolTableLevel::dump(TInfoSink &infoSink) const
{
	tLevel::const_iterator it;
	for (it = level.begin(); it != level.end(); ++it)
		(*it).second->dump(infoSink);
}

void TSymbolTable::dump(TInfoSink &infoSink) const
{
	for (int level = currentLevel(); level >= 0; --level) {
		infoSink.debug << "LEVEL " << level << "\n";
		table[level]->dump(infoSink);
	}
}

//
// Functions have buried pointers to delete.
//
TFunction::~TFunction()
{
	for (TParamList::iterator i = parameters.begin(); i != parameters.end();
			++i)
		delete (*i).type;
}

//
// Symbol table levels are a map of pointers to symbols that have to be deleted.
//
TSymbolTableLevel::~TSymbolTableLevel()
{
	for (tLevel::iterator it = level.begin(); it != level.end(); ++it)
		delete (*it).second;
}

//
// Change all function entries in the table with the non-mangled name
// to be related to the provided built-in operation.  This is a low
// performance operation, and only intended for symbol tables that
// live across a large number of compiles.
//
void TSymbolTableLevel::relateToOperator(const char* name, TOperator op)
{
	tLevel::iterator it;
	for (it = level.begin(); it != level.end(); ++it) {
		if ((*it).second->isFunction()) {
			TFunction* function = static_cast<TFunction*>((*it).second);
			if (function->getName() == name)
				function->relateToOperator(op);
		}
	}
}

TSymbol::TSymbol(const TSymbol& copyOf)
{
	name = NewPoolTString(copyOf.name->c_str());
	uniqueId = copyOf.uniqueId;
}

TVariable::TVariable(const TVariable& copyOf, TStructureMap& remapper) :
		TSymbol(copyOf)
{
	type.copyType(copyOf.type, remapper);
	userType = copyOf.userType;
	// for builtIn symbol table level, unionArray and arrayInformation pointers should be NULL
	assert(copyOf.arrayInformationType == 0);
	arrayInformationType = 0;

	if (copyOf.unionArray) {
		assert(!copyOf.type.getStruct());
		/*
		 if (!(copyOf.type.getObjectSize() == 1)) {
		 char* c = NULL;
		 *c = 5;
		 }

		 assert(copyOf.type.getObjectSize() == 1);
		 unionArray = new constUnion[1];
		 unionArray[0] = copyOf.unionArray[0];
		 */
		int i;
		unionArray = new constUnion[copyOf.type.getObjectSize()];
		for (i = 0; i < copyOf.type.getObjectSize(); i++) {
			unionArray[i] = copyOf.unionArray[i];
		}

	} else
		unionArray = 0;
}

TVariable* TVariable::clone(TStructureMap& remapper)
{
	TVariable *variable = new TVariable(*this, remapper);

	return variable;
}

TFunction::TFunction(const TFunction& copyOf, TStructureMap& remapper) :
		TSymbol(copyOf)
{
	for (unsigned int i = 0; i < copyOf.parameters.size(); ++i) {
		TParameter param;
		parameters.push_back(param);
		parameters.back().copyParam(copyOf.parameters[i], remapper);
	}

	returnType.copyType(copyOf.returnType, remapper);
	mangledName = copyOf.mangledName;
	op = copyOf.op;
	defined = copyOf.defined;
}

TFunction* TFunction::clone(TStructureMap& remapper)
{
	TFunction *function = new TFunction(*this, remapper);

	return function;
}

TSymbolTableLevel* TSymbolTableLevel::clone(TStructureMap& remapper)
{
	TSymbolTableLevel *symTableLevel = new TSymbolTableLevel();
	tLevel::iterator iter;
	for (iter = level.begin(); iter != level.end(); ++iter) {
		symTableLevel->insert(*iter->second->clone(remapper));
	}

	return symTableLevel;
}

void TSymbolTable::copyTable(const TSymbolTable& copyOf)
{
	TStructureMap remapper;
	uniqueId = copyOf.uniqueId;
	for (unsigned int i = 0; i < copyOf.table.size(); ++i) {
		table.push_back(copyOf.table[i]->clone(remapper));
	}
}
