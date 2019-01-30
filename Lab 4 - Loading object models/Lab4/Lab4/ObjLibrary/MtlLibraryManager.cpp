//
//  MtlLibraryManager.cpp
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
#include <vector>
#include <iostream>
#include <fstream>

#include "ObjStringParsing.h"
#include "MtlLibrary.h"
#include "MtlLibraryManager.h"

using namespace std;
using namespace ObjLibrary;
using namespace ObjLibrary::ObjStringParsing;
using namespace ObjLibrary::MtlLibraryManager;
namespace
{
	std::vector<MtlLibrary*> g_mtl_libraries;
	MtlLibrary g_empty;
}



unsigned int MtlLibraryManager :: getCount ()
{
	return g_mtl_libraries.size();
}

MtlLibrary& MtlLibraryManager :: get (unsigned int index)
{
	assert(index < getCount());

	return *(g_mtl_libraries[index]);
}

bool MtlLibraryManager :: isLoaded (const char* a_name)
{
	assert(a_name != NULL);

	return isLoaded(string(a_name));
}

bool MtlLibraryManager :: isLoaded (const std::string& name)
{
	string lower = toLowercase(name);

	for(unsigned int i = 0; i < g_mtl_libraries.size(); i++)
		if(g_mtl_libraries[i]->getFileNameWithPathLowercase() == lower)
			return true;
	return false;
}

MtlLibrary& MtlLibraryManager :: get (const char* a_name)
{
	assert(a_name != NULL);

	return get(string(a_name), cerr);
}

MtlLibrary& MtlLibraryManager :: get (const char* a_name, const string& logfile)
{
	assert(a_name != NULL);
	assert(logfile != "");

	ofstream logstream(logfile.c_str());
	MtlLibrary* p_library = &(get(string(a_name), logstream));
	logstream.close();

	return *p_library;
}

MtlLibrary& MtlLibraryManager :: get (const char* a_name, ostream& r_logstream)
{
	assert(a_name != NULL);

	return get(string(a_name), r_logstream);
}

MtlLibrary& MtlLibraryManager :: get (const string& name)
{
	return get(name, cerr);
}

MtlLibrary& MtlLibraryManager :: get (const string& name, const string& logfile)
{
	assert(logfile != "");

	ofstream logstream(logfile.c_str());
	MtlLibrary* p_library = &(get(name, logstream));
	logstream.close();

	return *p_library;
}

MtlLibrary& MtlLibraryManager :: get (const string& name, ostream& r_logstream)
{
	string lower = toLowercase(name);

	for(unsigned int i = 0; i < g_mtl_libraries.size(); i++)
		if(g_mtl_libraries[i]->getFileNameWithPathLowercase() == lower)
			return *(g_mtl_libraries[i]);

	if(endsWith(lower, ".mtl"))
		return add(MtlLibrary(name, r_logstream));
	else
		return g_empty;
}

bool MtlLibraryManager :: isMaterial (const char* a_name, const char* a_material)
{
	assert(a_name != NULL);
	assert(a_material != NULL);

	return isMaterial(string(a_name), string(a_material), cerr);
}

bool MtlLibraryManager :: isMaterial (const char* a_name, const char* a_material, const string& logfile)
{
	assert(a_name != NULL);
	assert(a_material != NULL);
	assert(logfile != "");

	ofstream logstream(logfile.c_str());
	bool exists = isMaterial(string(a_name), string(a_material), logstream);
	logstream.close();

	return exists;
}

bool MtlLibraryManager :: isMaterial (const char* a_name, const char* a_material, ostream& r_logstream)
{
	assert(a_name != NULL);
	assert(a_material != NULL);

	return isMaterial(string(a_name), string(a_material), r_logstream);
}

bool MtlLibraryManager :: isMaterial (const string& name, const string& material)
{
	return isMaterial(name, material, cerr);
}

bool MtlLibraryManager :: isMaterial (const string& name, const string& material, const string& logfile)
{
	assert(logfile != "");

	ofstream logstream(logfile.c_str());
	bool exists = isMaterial(name, material, logstream);
	logstream.close();

	return exists;
}

bool MtlLibraryManager :: isMaterial (const string& name, const string& material, ostream& r_logstream)
{
	// isMaterial performs case-insensitive check
	return get(name, r_logstream).isMaterial(material);
}

Material* MtlLibraryManager :: getMaterial (const char* a_name, const char* a_material)
{
	assert(a_name != NULL);
	assert(a_material != NULL);

	return getMaterial(string(a_name), string(a_material), cerr);
}

Material* MtlLibraryManager :: getMaterial (const char* a_name, const char* a_material, const string& logfile)
{
	assert(a_name != NULL);
	assert(a_material != NULL);
	assert(logfile != "");

	ofstream logstream(logfile.c_str());
	Material* p_material = getMaterial(string(a_name), string(a_material), logstream);
	logstream.close();

	return p_material;
}

Material* MtlLibraryManager :: getMaterial (const char* a_name, const char* a_material, ostream& r_logstream)
{
	assert(a_name != NULL);
	assert(a_material != NULL);

	return getMaterial(string(a_name), string(a_material), r_logstream);
}

Material* MtlLibraryManager :: getMaterial (const string& name, const string& material)
{
	return getMaterial(name, material, cerr);
}

Material* MtlLibraryManager :: getMaterial (const string& name, const string& material, const string& logfile)
{
	assert(logfile != "");

	ofstream logstream(logfile.c_str());
	Material* p_material = getMaterial(name, material, logstream);
	logstream.close();

	return p_material;
}

Material* MtlLibraryManager :: getMaterial (const string& name, const string& material, ostream& r_logstream)
{
	// getMaterial performs case-insensitive check
	return get(name, r_logstream).getMaterial(material);
}

MtlLibrary& MtlLibraryManager :: add (const MtlLibrary& mtl_library)
{
	assert(!isLoaded(mtl_library.getFileNameWithPathLowercase()));

	unsigned int index = g_mtl_libraries.size();
	g_mtl_libraries.push_back(new MtlLibrary(mtl_library));

	return *(g_mtl_libraries[index]);
}

void MtlLibraryManager :: unloadAll ()
{
	for(unsigned int i = 0; i < g_mtl_libraries.size(); i++)
		delete g_mtl_libraries[i];
	g_mtl_libraries.clear();
}

void MtlLibraryManager :: loadDisplayTextures ()
{
	// such simple code for such a powerful command...

	for(unsigned int i = 0; i < g_mtl_libraries.size(); i++)
		g_mtl_libraries[i]->loadDisplayTextures();
}

void MtlLibraryManager :: loadAllTextures ()
{
	// such simple code for such a powerful command...

	for(unsigned int i = 0; i < g_mtl_libraries.size(); i++)
		g_mtl_libraries[i]->loadAllTextures();
}
