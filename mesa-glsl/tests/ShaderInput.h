/*
 * ShaderInput.h
 *
 *  Created on: 12.02.2014
 */

#ifndef TEST_SHADERINPUT_H_
#define TEST_SHADERINPUT_H_

#include "interface/ShaderHolder.h"
#include "Rules.h"
#include <map>
#include <vector>
#include <string>
#include <cppunit/TestAssert.h>

#define SHADERS_PER_PROGRAM 3

typedef std::map<std::string, ShaderHolder*> ShadersList;
typedef std::vector<std::string> ResultsList;
typedef std::map<std::string, ResultsList> ResultsListMap;
typedef std::map<std::string, std::vector<ResultsList>> ResultsFilesMap;
typedef std::vector<ResultsList>::iterator ResultsFilesIterator;
typedef std::map<int, TestRule> RulesList;
typedef std::map<std::string, RulesList> RulesMap;

class Resource {
public:
	static void setPath(std::string p)
	{
		path = p;
	}

protected:
	static std::string path;
};

class ShaderInput: public Resource {
public:
	static void free();
	static ShaderHolder* getShader(std::string name)
	{
		ShadersList::iterator shader = shaders.find(name);
		if (shader != shaders.end())
			return shader->second;
		ShaderHolder* holder = load(name);
		shaders[name] = holder;
		return holder;
	}


private:
	static ShaderHolder* load(std::string);
	static ShadersList shaders;
};

class ResultComparator: public Resource {
public:
	ResultComparator() :
			current(NULL), line(-1)
	{
		file = file_end = dummy_files.end();
		file_num = 0;
	}

	int getFileNum()
	{
		return file_num;
	}

	void setCurrent(std::string name, std::string unit)
	{
		line = -1;
		file_num = 0;
		current_file = name + "." + unit;
		file = file_end = dummy_files.end();
		states.clear();
		current = getResults(current_file);
		if (!current && getResultsFile(current_file, file, file_end)) {
			current = &(*file);
			file_num++;
		}
	}

	bool nameMatch(std::string name, std::string unit)
	{
		return !current_file.compare(name + "." + unit);
	}

	bool nextFile()
	{
		if (file == file_end)
			return false;

		current = NULL;
		file++;
		file_num++;
		line = -1;

		// Set new file if it exists or reset it
		if (file == file_end) {
			file = file_end = dummy_files.end();
			std::stringstream ss;
			ss << "Request for new file while it is not exists.\n" << current_file << "/"
					<< file_num;
			CPPUNIT_FAIL(ss.str());
			return false;
		}

		current = &(*file);
		return true;
	}

	void applyRules(AstShader* sh) {
		int dummy;
		applyRules(sh, dummy);
	}

	void applyRules(AstShader* sh, int& behaviour)
	{
		TestRule* rule = getRules(current_file, file_num);
		if (!rule)
			return;

		if (rule->stateAction())
			rule->apply(&states, sh);

		if (rule->jumpAction()) {
			int to = rule->getJump();
			if (to < file_num)
				while (to < file_num--)
					file--;
			else
				while (to > file_num++)
					file++;
		}

		if (rule->bhvAction())
			behaviour = rule->getBehaviour();
	}

	void compareNext(std::string cmpline)
	{
		CPPUNIT_ASSERT_MESSAGE("No comparison results", current && !current->empty());
		std::string orig_line = "";
		if (++line < (int)current->size())
			orig_line = current->at(line);
		std::stringstream ss;
		ss << "File: " << current_file;
		if (file_num)
			ss << "/" << file_num;
		ss << "\n" << "got line\n" << cmpline << "\nexcepted\n" << orig_line << "\nat line: "
				<< (line + 1);
		CPPUNIT_ASSERT_MESSAGE(ss.str().c_str(), !orig_line.compare(cmpline));
	}

	static void loadResults(std::string name, std::string unit);

protected:
	static ResultsList* getResults(std::string name)
	{
		auto it = results.find(name);
		if (it != results.end())
			return &it->second;
		return NULL;
	}

	bool getResultsFile(std::string name, ResultsFilesIterator& f, ResultsFilesIterator& end)
	{
// Search if it was directory
		auto fit = results_dirs.find(name);
		if (fit != results_dirs.end()) {
			f = fit->second.begin();
			end = fit->second.end();
			return true;
		}
		return false;
	}

	TestRule* getRules(std::string name, int rule_line)
	{
		auto rit = rules.find(name);
		if (rit != rules.end()) {
			RulesList& l = rit->second;
			auto rlit = l.find(rule_line);
			if (rlit != l.end())
				return &rlit->second;
		}
		return NULL;
	}

private:
	static ResultsFilesMap results_dirs;
	static ResultsListMap results;
	static RulesMap rules;
	StatesMap states;
	ResultsList* current;
	ResultsFilesIterator file;
	ResultsFilesIterator file_end;
	static std::vector<ResultsList> dummy_files;
	std::string current_file;
	int file_num;
	int line;
}
;

#endif /* TEST_SHADERINPUT_H_ */
