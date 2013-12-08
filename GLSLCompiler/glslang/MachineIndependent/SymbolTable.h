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

#ifndef _SYMBOL_TABLE_INCLUDED_
#define _SYMBOL_TABLE_INCLUDED_

//
// Symbol table for parsing.  Has these design characteristics:
//
// * Same symbol table can be used to compile many shaders, to preserve
//   effort of creating and loading with the large numbers of built-in
//   symbols.
//
// * Name mangling will be used to give each function a unique name
//   so that symbol table lookups are never ambiguous.  This allows
//   a simpler symbol table structure.
//
// * Pushing and popping of scope, so symbol table will really be a stack
//   of symbol tables.  Searched from the top, with new inserts going into
//   the top.
//
// * Constants:  Compile time constant symbols will keep their values
//   in the symbol table.  The parser can substitute constants at parse
//   time, including doing constant folding and constant propagation.
//
// * No temporaries:  Temporaries made from operations (+, --, .xy, etc.)
//   are tracked in the intermediate representation, not the symbol table.
//

#include "Include/Common.h"
#include "Include/intermediate.h"
#include "Include/InfoSink.h"
#include "Public/ShaderLang.h"

//
// Symbol base class.  (Can build functions or variables out of these...)
//
class TSymbol {
public:
	POOL_ALLOCATOR_NEW_DELETE(GlobalPoolAllocator)
	TSymbol(const TString *n) :
			name(n)
	{
	}
	virtual ~TSymbol()
	{ /* don't delete name, it's from the pool */
	}
	const TString& getName() const
	{
		return *name;
	}
	virtual const TString& getMangledName() const
	{
		return getName();
	}
	virtual bool isFunction() const
	{
		return false;
	}
	virtual bool isVariable() const
	{
		return false;
	}
	void setUniqueId(int id)
	{
		uniqueId = id;
	}
	int getUniqueId() const
	{
		return uniqueId;
	}
	virtual void dump(TInfoSink &infoSink) const = 0;
	TSymbol(const TSymbol&);
	virtual TSymbol* clone(TStructureMap& remapper) = 0;

protected:
	const TString *name;
	unsigned int uniqueId;      // For real comparing during code generation
};

//
// Variable class, meaning a symbol that's not a function.
//
// There could be a separate class heirarchy for Constant variables;
// Only one of int, bool, or float, (or none) is correct for
// any particular use, but it's easy to do this way, and doesn't
// seem worth having separate classes, and "getConst" can't simply return
// different values for different types polymorphically, so this is
// just simple and pragmatic.
//
class TVariable: public TSymbol {
public:
	TVariable(const TString *name, const TType& t, bool uT = false) :
			TSymbol(name), type(t), userType(uT), unionArray(0), arrayInformationType(
					0)
	{
	}
	virtual ~TVariable()
	{
	}
	virtual bool isVariable() const
	{
		return true;
	}
	TType& getType()
	{
		return type;
	}
	const TType& getType() const
	{
		return type;
	}
	bool isUserType() const
	{
		return userType;
	}
	void changeQualifier(TQualifier qualifier)
	{
		type.changeQualifier(qualifier);
	}
	void updateArrayInformationType(TType *t)
	{
		arrayInformationType = t;
	}
	TType* getArrayInformationType()
	{
		return arrayInformationType;
	}

	virtual void dump(TInfoSink &infoSink) const;

	constUnion* getConstPointer()
	{
		if (!unionArray)
			unionArray = new constUnion[type.getObjectSize()];

		return unionArray;
	}

	constUnion* getConstPointer() const
	{
		return unionArray;
	}

	void shareConstPointer(constUnion *constArray)
	{
		delete unionArray;
		unionArray = constArray;
	}
	TVariable(const TVariable&, TStructureMap& remapper);  // copy constructor
	virtual TVariable* clone(TStructureMap& remapper);

	// Returns if variable is debuggable
	bool isShVariable(void)
	{
		return (type.getBasicType() == EbtFloat || type.getBasicType() == EbtInt
				|| type.getBasicType() == EbtUInt
				|| type.getBasicType() == EbtBool
				|| type.getBasicType() == EbtStruct
				|| IsSampler(type.getBasicType()));
	}

	// Returns a variable description for external use
	ShVariable* getShVariable(void)
	{
		ShVariable* v = type.getShVariable();
		v->uniqueId = uniqueId;
		v->name = (char*) malloc(strlen(name->c_str()) + 1);
		strcpy(v->name, name->c_str());
		return v;
	}

protected:
	TType type;
	bool userType;
	// we are assuming that Pool Allocator will free the memory allocated to unionArray
	// when this object is destroyed
	constUnion *unionArray;
	TType *arrayInformationType;  // this is used for updating maxArraySize in all the references to a given symbol
};

//
// The function sub-class of symbols and the parser will need to
// share this definition of a function parameter.
//
struct TParameter {
	TString *name;
	TType* type;
	void copyParam(const TParameter& param, TStructureMap& remapper)
	{
		name = NewPoolTString(param.name->c_str());
		type = param.type->clone(remapper);
	}
};

//
// The function sub-class of a symbol.
//
class TFunction: public TSymbol {
public:
	TFunction(TOperator o) :
			TSymbol(0), returnType(TType(EbtVoid)), op(o), defined(false)
	{
	}
	TFunction(const TString *name, TType& retType, TOperator tOp = EOpNull) :
			TSymbol(name), returnType(retType), mangledName(*name + '('), op(
					tOp), defined(false)
	{
	}
	virtual ~TFunction();
	virtual bool isFunction() const
	{
		return true;
	}

	void addParameter(TParameter& p)
	{
		parameters.push_back(p);
		mangledName = mangledName + p.type->getMangledName();
	}

	const TString& getMangledName() const
	{
		return mangledName;
	}
	void setMangledName(TString mname)
	{
		mangledName = mname;
	}
	const TType& getReturnType() const
	{
		return returnType;
	}
	TType* getReturnTypePointer()
	{
		return &returnType;
	}
	void relateToOperator(TOperator o)
	{
		op = o;
	}
	TOperator getBuiltInOp() const
	{
		return op;
	}
	void setDefined()
	{
		defined = true;
	}
	bool isDefined()
	{
		return defined;
	}

	int getParamCount() const
	{
		return static_cast<int>(parameters.size());
	}
	TParameter& operator [](int i)
	{
		return parameters[i];
	}
	const TParameter& operator [](int i) const
	{
		return parameters[i];
	}

	virtual void dump(TInfoSink &infoSink) const;
	TFunction(const TFunction&, TStructureMap& remapper);
	virtual TFunction* clone(TStructureMap& remapper);

protected:
	typedef TVector<TParameter> TParamList;
	TParamList parameters;
	TType returnType;
	TString mangledName;
	TOperator op;
	bool defined;
};

class TSymbolTableLevel {
public:
	POOL_ALLOCATOR_NEW_DELETE(GlobalPoolAllocator)
	TSymbolTableLevel()
	{
	}
	~TSymbolTableLevel();

	bool insert(TSymbol& symbol)
	{
		//
		// returning true means symbol was added to the table
		//
		tInsertResult result;
		result = level.insert(tLevelPair(symbol.getMangledName(), &symbol));

		return result.second;
	}

	TSymbol* find(const TString& name) const
	{
		tLevel::const_iterator it = level.find(name);
		if (it == level.end())
			return 0;
		else
			return (*it).second;
	}

	void relateToOperator(const char* name, TOperator op);
	void dump(TInfoSink &infoSink) const;
	TSymbolTableLevel* clone(TStructureMap& remapper);

	size_t getSize(void)
	{
		return level.size();
	}

	void getEntry(int id, TString **name, TSymbol **sym)
	{
		if ((0 <= id) && (id < (int) level.size())) {
			int i;
			tLevel::const_iterator it = level.begin();
			for (i = 0; i < id; i++, it++)
				;
			*name = (TString*) &(it->first);
			*sym = it->second;
		} else {
			*name = NULL;
			*sym = NULL;
		}
	}

protected:
	typedef std::map<TString, TSymbol*, std::less<TString>,
			pool_allocator<std::pair<const TString, TSymbol*> > > tLevel;
	typedef const tLevel::value_type tLevelPair;
	typedef std::pair<tLevel::iterator, bool> tInsertResult;

	tLevel level;
};

class TSymbolTable {
public:
	TSymbolTable() :
			uniqueId(0)
	{
		//
		// The symbol table cannot be used until push() is called, but
		// the lack of an initial call to push() can be used to detect
		// that the symbol table has not been preloaded with built-ins.
		//
	}

	TSymbolTable(TSymbolTable& symTable)
	{
		table.push_back(symTable.table[0]);
		uniqueId = symTable.uniqueId;
	}

	~TSymbolTable()
	{
		// level 0 is always built In symbols, so we never pop that out
		while (table.size() > 1)
			pop();
	}

	//
	// When the symbol table is initialized with the built-ins, there should
	// 'push' calls, so that built-ins are at level 0 and the shader
	// globals are at level 1.
	//
	bool isEmpty()
	{
		return table.size() == 0;
	}
	bool atBuiltInLevel()
	{
		return atSharedBuiltInLevel() || atDynamicBuiltInLevel();
	}
	bool atSharedBuiltInLevel()
	{
		return table.size() == 1;
	}
	bool atGlobalLevel()
	{
		return table.size() <= 3;
	}
	void push()
	{
		table.push_back(new TSymbolTableLevel);
	}

	void pop()
	{
		delete table[currentLevel()];
		table.pop_back();
	}

	bool insert(TSymbol& symbol)
	{
		symbol.setUniqueId(++uniqueId);
		return table[currentLevel()]->insert(symbol);
	}

	TSymbol* find(const TString& name, bool* builtIn = 0, bool *sameScope = 0)
	{
		int level = currentLevel();
		TSymbol* symbol;
		do {
			symbol = table[level]->find(name);
			--level;
		} while (symbol == 0 && level >= 0);
		level++;
		if (builtIn)
			*builtIn = level == 0;
		if (sameScope)
			*sameScope = level == currentLevel();
		return symbol;
	}

	TSymbolTableLevel* getGlobalLevel()
	{
		assert(table.size() >= 3);
		return table[2];
	}
	void relateToOperator(const char* name, TOperator op)
	{
		table[0]->relateToOperator(name, op);
	}
	int getMaxSymbolId()
	{
		return uniqueId;
	}
	void dump(TInfoSink &infoSink) const;
	void copyTable(const TSymbolTable& copyOf);

	TSymbolTableLevel* getLevel(int l)
	{
		if (0 <= l && l <= (int) table.size())
			return table[l];
		else
			return NULL;
	}

protected:
	int currentLevel() const
	{
		return static_cast<int>(table.size()) - 1;
	}
	bool atDynamicBuiltInLevel()
	{
		return table.size() == 2;
	}

	std::vector<TSymbolTableLevel*> table;
	int uniqueId;     // for unique identification in code generation
};

#endif // _SYMBOL_TABLE_INCLUDED_
