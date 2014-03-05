/*
 * ShaderInput.h
 *
 *  Created on: 12.02.2014
 */

#ifndef TEST_SHADERINPUT_H_
#define TEST_SHADERINPUT_H_

#include "glslang/Interface/ShaderHolder.h"
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
	}

	void setCurrent(std::string name, std::string unit)
	{
		line = -1;
		file = file_end = dummy_files.end();
		current = getResults(name, unit);
		if (!current && getResultsFile(name, unit, file, file_end))
			current = &(*file);
	}

	bool nextFile()
	{
		if (file == file_end)
			return false;

		current = NULL;
		CPPUNIT_ASSERT_MESSAGE("No next file", file != file_end);
		file++;

		// Set new file if it exists or reset it
		if (file == file_end) {
			file = file_end = dummy_files.end();
			return false;
		}

		current = &(*file);
		return true;
	}

	void compareNext(std::string cmpline)
	{
		CPPUNIT_ASSERT_MESSAGE("No comparison results", current && !current->empty());
		std::string orig_line = current->at(++line);
		std::stringstream ss;
		ss << "got line\n" << cmpline << "\nexcepted\n" << orig_line << "\nat line: "
				<< (line + 1);
		CPPUNIT_ASSERT_MESSAGE(ss.str().c_str(), !orig_line.compare(cmpline));
	}

	static void loadResults(std::string name, std::string unit);

protected:
	static ResultsList* getResults(std::string name, std::string unit)
	{
		std::map<std::string, ResultsListMap>::iterator it = results.find(name);
		if (it != results.end()) {
			ResultsListMap::iterator rit = it->second.find(unit);
			if (rit != it->second.end())
				 return &rit->second;
		}
		return NULL;
	}

	bool getResultsFile(std::string name, std::string unit,
			ResultsFilesIterator& f, ResultsFilesIterator& end)
	{
		// Search if it was directory
		std::map<std::string, ResultsFilesMap>::iterator fit = results_dirs.find(name);
		if (fit != results_dirs.end()) {
			ResultsFilesMap::iterator rit = fit->second.find(unit);
			if (rit != fit->second.end()){
				f = rit->second.begin();
				end = rit->second.end();
				return true;
			}
		}
		return false;
	}


private:
	static std::map<std::string, ResultsFilesMap> results_dirs;
	static std::map<std::string, ResultsListMap> results;
	ResultsList* current;
	ResultsFilesIterator file;
	ResultsFilesIterator file_end;
	static std::vector<ResultsList> dummy_files;
	int line;
};

#endif /* TEST_SHADERINPUT_H_ */
