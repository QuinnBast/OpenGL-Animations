//
//  ObjStringParsing.cpp
//
//  This file is part of the ObjLibrary, by Richard Hamilton,
//    which is copyright Hamilton 2009-2016.
//
//  You may use these files for any purpose as long as you do
//    not explicitly claim them as your own work or object to
//    other people using them.
//
//  If you are destributing the source files, you must not
//    remove this notice.  If you are only destributing compiled
//    code, no credit is required.
//
//  A (theoretically) up-to-date version of the ObjLibrary can
//    be found at:
//  http://infiniplix.ca/resources/obj_library/
//

#include <cassert>
#include <string>

#include "ObjStringParsing.h"

using namespace std;
using namespace ObjLibrary;
using namespace ObjLibrary::ObjStringParsing;



size_t ObjStringParsing :: nextToken (const string& str, size_t current)
{
	size_t length = str.length();
	bool seen_whitespace = false;

	// return out of loop when next token is found
	for(size_t i = current; i < length; i++)
	{
		if(seen_whitespace)
		{
			if(!isspace(str[i]))
				return i;
		}
		else
		{
			if(isspace(str[i]))
				seen_whitespace = true;
		}
	}

	// you only get here if there is no next token
	return string::npos;
}

size_t ObjStringParsing :: getTokenLength (const string& str, size_t current)
{
	size_t length = str.length();

	// return out of loop when next token is found
	for(size_t i = current; i < length; i++)
	{
		if(isspace(str[i]))
			return i - current;
	}

	// you only get here if there is no next token
	return length - current;
}

size_t ObjStringParsing :: nextSlashInToken (const string& str, size_t current)
{
	size_t length = str.length();

	// return out of loop when next token is found
	for(size_t i = current; i < length; i++)
	{
		if(str[i] == '/')
			return i;
		else if(isspace(str[i]))
			return string::npos;
	}

	// you only get here if there is no next token
	return string::npos;
}



string ObjStringParsing :: toLowercase (const string& str)
{
	unsigned int length = str.length();
	string result;

	result.resize(length);
	for(unsigned int i = 0; i < length; i++)
	{
		char c = str[i];
		if(c >= 'A' && c <= 'Z')
			result[i] = c - 'A' + 'a';
		else
			result[i] = c;
	}

	return result;
}

string ObjStringParsing :: whitespaceToSpaces (const string& str)
{
	unsigned int length = str.length();
	string result = str;

	for(unsigned int i = 0; i < length; i++)
		if(isspace(result[i]))
			result[i] = ' ';

	return result;
}



bool ObjStringParsing :: endsWith (const std::string& str, const char* a_end)
{
	assert(a_end != NULL);

	return endsWith(str, string(a_end));
}

bool ObjStringParsing :: endsWith (const std::string& str, const std::string& end)
{
	unsigned int str_length = str.length();
	unsigned int end_length = end.length();

	if(str_length < end_length)
		return false;

	for(unsigned int i = 1; i <= end_length; i++)
		if(end[end_length - i] != str[str_length - i])
			return false;

	return true;
}

bool ObjStringParsing :: startsWith (const std::string& str, const char* a_start)
{
	assert(a_start != NULL);

	return startsWith(str, string(a_start));
}

bool ObjStringParsing :: startsWith (const std::string& str, const std::string& start)
{
	unsigned int   str_length =   str.length();
	unsigned int start_length = start.length();

	if(str_length < start_length)
		return false;

	for(unsigned int i = 0; i < start_length; i++)
		if(start[i] != str[i])
			return false;

	return true;
}



bool ObjStringParsing :: isValidPath (const std::string& path)
{
	// the empty string is a path to the current directory, so is always legal
	if(path == "")
		return true;

	assert(!path.empty());
	assert(path.size() > 0);
	// a leading slash indicates an absolute path, so is legal (if a bad idea)

	// a path must end with a slash (or backslash)
	if(path.back() == '/')
		return true;
	if(path.back() == '\\')
		return true;
	return false;
}
