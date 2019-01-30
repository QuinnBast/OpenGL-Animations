//
//  TextureManager.cpp
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

#include "ObjSettings.h"

#ifdef OBJ_LIBRARY_SHADER_DISPLAY
	#include "../GetGlutWithShaders.h"
#else
	#include "../GetGlut.h"
#endif

#include "Vector3.h"
#include "ObjStringParsing.h"
#include "Texture.h"
#include "TextureBmp.h"
#include "TextureManager.h"

#ifdef OBJ_LIBRARY_LOAD_PNG_TEXTURES
	#include "../glpng.h"
#endif

using namespace std;
using namespace ObjLibrary;
using namespace ObjLibrary::ObjStringParsing;
using namespace ObjLibrary::TextureManager;
namespace
{
	struct TextureData
	{
		string  m_name;
		Texture m_texture;
	};

	// some sort of hash table would be nice
	//  -> convert to map<> when C++11 becomes more widely
	//     supported?
	vector<TextureData*> gvp_textures;

	Texture g_white;

	//
	//  getDummyTetxure
	//
	//  Purpose: To retrieve a reference to the dummy texture.
	//  Parameter(s): N/A
	//  Precondition(s): N/A
	//  Returns: A reference to the dummy texture.
	//  Side Effect: If the dummy texture has not been
	//               generated, it is generated.
	//
	const Texture& getDummyTexture ()
	{
		if(!g_white.isSet())
		{
			TextureBmp white;
			g_white.set(white.addToOpenGL());
		}

		return g_white;
	}



	// global values used with the callback function below
	unsigned char g_transparent_red   = 0x00;
	unsigned char g_transparent_green = 0x00;
	unsigned char g_transparent_blue  = 0x00;

	//
	//  pngAlphaStencilCallback
	//
	//  Purpose: To calculate the alpha channel for a pixel with
	//           the specified red, green, and blue channels.
	//           This function is used when loading an image
	//           with a single transparent colour.  All colour
	//           channel values are unsigned integers in the
	//           range [0x00, 0xFF].
	//  Parameter(s):
	//    <1> red: The red value for the pixel
	//    <2> green: The green value for the pixel
	//    <3> blue: The blue value for the pixel
	//  Precondition(s): N/A
	//  Returns: A value for the alpha channel, in the range
	//           [0x00, 0xFF].
	//  Side Effect: N/A
	//
	unsigned char pngAlphaStencilCallback (unsigned char red,
	                                       unsigned char green,
	                                       unsigned char blue)
	{
		return (red   == g_transparent_red   &&
		        green == g_transparent_green &&
		        blue  == g_transparent_blue) ? 0x00 : 0xFF;
	}
}



unsigned int TextureManager :: getCount ()
{
	return gvp_textures.size();
}

const std::string& getName (unsigned int index)
{
	assert(index < getCount());

	assert(index < gvp_textures.size());
	assert(gvp_textures[index] != NULL);
	return gvp_textures[index]->m_name;
}

const Texture& TextureManager :: get (unsigned int index)
{
	assert(index < getCount());

	assert(index < gvp_textures.size());
	assert(gvp_textures[index] != NULL);
	return gvp_textures[index]->m_texture;
}

const Texture& TextureManager :: get (const char* a_name)
{
	assert(a_name != NULL);

	return get(string(a_name));
}

const Texture& TextureManager :: get (const string& name)
{
	unsigned int index = getIndex(name);
	if(index == TEXTURE_INDEX_INVALID)
		index = load(name, cerr);

	if(index == TEXTURE_INDEX_INVALID)
		return getDummyTexture();
	else
	{
		assert(index < gvp_textures.size());
		assert(gvp_textures[index] != NULL);
		assert(toLowercase(gvp_textures[index]->m_name) == toLowercase(name));
		return gvp_textures[index]->m_texture;
	}
}

void TextureManager :: activate (unsigned int index)
{
	assert(index < getCount());

	assert(index < gvp_textures.size());
	assert(gvp_textures[index] != NULL);
	gvp_textures[index]->m_texture.activate();
}

void TextureManager :: activate (const char* a_name)
{
	assert(a_name != NULL);

	get(string(a_name)).activate();
}

void TextureManager :: activate (const std::string& name)
{
	get(name).activate();
}



bool TextureManager :: isLoaded (const char* a_name)
{
	assert(a_name != NULL);

	return isLoaded(string(a_name));
}

bool TextureManager :: isLoaded (const string& name)
{
	return (getIndex(name) != TEXTURE_INDEX_INVALID);
}

unsigned int TextureManager :: getIndex (const char* a_name)
{
	assert(a_name != NULL);

	return isLoaded(string(a_name));
}

unsigned int TextureManager :: getIndex (const std::string& name)
{
	string lower = toLowercase(name);

	for(unsigned int i = 0; i < gvp_textures.size(); i++)
	{
		assert(gvp_textures[i] != NULL);
		if(toLowercase(gvp_textures[i]->m_name) == lower)
			return i;
	}
	return TEXTURE_INDEX_INVALID;
}

bool TextureManager :: isDummyTexture (const Texture& texture)
{
	if(texture == getDummyTexture())
		return true;
	else
		return false;
}



unsigned int TextureManager :: add (unsigned int opengl_name, const char* a_name)
{
	assert(opengl_name != 0);
	assert(a_name != NULL);
	assert(!isLoaded(a_name));

	return add(Texture(opengl_name), string(a_name));
}

unsigned int TextureManager :: add (unsigned int opengl_name, const string& name)
{
	assert(opengl_name != 0);
	assert(!isLoaded(name));

	return add(Texture(opengl_name), name);
}

unsigned int TextureManager :: add (const Texture& texture, const char* a_name)
{
	assert(texture.isSet());
	assert(a_name != NULL);
	assert(!isLoaded(a_name));

	return add(texture, string(a_name));
}

unsigned int TextureManager :: add (const Texture& texture, const string& name)
{
	assert(texture.isSet());
	assert(!isLoaded(name));

	unsigned int texture_count = gvp_textures.size();

	gvp_textures.push_back(new TextureData);  //  USE INITIALIZING CONSTRUCTOR  <|>
	assert(texture_count < gvp_textures.size());
	assert(gvp_textures[texture_count] != NULL);
	gvp_textures[texture_count]->m_name    = name;
	gvp_textures[texture_count]->m_texture = texture;

	return texture_count;
}



unsigned int TextureManager :: load (const char* a_name)
{
	assert(a_name != NULL);
	assert(!isLoaded(a_name));

	return load(string(a_name), cerr);
}

unsigned int TextureManager :: load (const char* a_name, const string& logfile)
{
	assert(a_name != NULL);
	assert(!isLoaded(a_name));
	assert(logfile != "");

	ofstream logstream(logfile.c_str());
	return load(string(a_name), logstream);
	// logstream.close() will be called by ofstream destructor
}

unsigned int TextureManager :: load (const char* a_name, ostream& r_logstream)
{
	assert(a_name != NULL);
	assert(!isLoaded(a_name));

	return load(string(a_name), r_logstream);
}

unsigned int TextureManager :: load (const string& name)
{
	assert(!isLoaded(name));

	return load(name, cerr);
}

unsigned int TextureManager :: load (const string& name,
                                     const string& logfile)
{
	assert(!isLoaded(name));
	assert(logfile != "");

	ofstream logstream(logfile.c_str());
	return load(name, logstream);
	// logstream.close() will be called by ofstream destructor
}

unsigned int TextureManager :: load (const string& name,
                                     ostream& r_logstream)
{
	assert(!isLoaded(name));

	//  All gets, activates, and loads without wrapping and
	//    mag/minification parameters go through this line.
#ifdef OBJ_LIBRARY_LINEAR_TEXTURE_INTERPOLATION
	return load(name, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, r_logstream);
#else
	return load(name, GL_REPEAT, GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST, r_logstream);
#endif
}

unsigned int TextureManager :: load (const char* a_name,
                                     unsigned int wrap_s,
                                     unsigned int wrap_t,
                                     unsigned int mag_filter,
                                     unsigned int min_filter)
{
	assert(a_name != NULL);
	assert(!isLoaded(a_name));
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	assert(wrap_s == GL_REPEAT || wrap_s == GL_CLAMP);
	assert(wrap_t == GL_REPEAT || wrap_t == GL_CLAMP);
#else
	assert(wrap_s == GL_REPEAT ||
	       wrap_s == GL_MIRRORED_REPEAT ||
	       wrap_s == GL_CLAMP_TO_EDGE ||
	       wrap_s == GL_CLAMP_TO_BORDER);
	assert(wrap_t == GL_REPEAT ||
	       wrap_t == GL_MIRRORED_REPEAT ||
	       wrap_t == GL_CLAMP_TO_EDGE ||
	       wrap_t == GL_CLAMP_TO_BORDER);
#endif
	assert(mag_filter == GL_NEAREST ||
	       mag_filter == GL_LINEAR);
	assert(min_filter == GL_NEAREST ||
	       min_filter == GL_LINEAR ||
	       min_filter == GL_NEAREST_MIPMAP_NEAREST ||
	       min_filter == GL_NEAREST_MIPMAP_LINEAR ||
	       min_filter == GL_LINEAR_MIPMAP_NEAREST ||
	       min_filter == GL_LINEAR_MIPMAP_LINEAR);

	return load(string(a_name), wrap_s, wrap_t, mag_filter, min_filter, cerr);
}

unsigned int TextureManager :: load (const char* a_name,
                                     unsigned int wrap_s,
                                     unsigned int wrap_t,
                                     unsigned int mag_filter,
                                     unsigned int min_filter,
                                     const std::string& logfile)
{
	assert(a_name != NULL);
	assert(!isLoaded(a_name));
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	assert(wrap_s == GL_REPEAT || wrap_s == GL_CLAMP);
	assert(wrap_t == GL_REPEAT || wrap_t == GL_CLAMP);
#else
	assert(wrap_s == GL_REPEAT ||
	       wrap_s == GL_MIRRORED_REPEAT ||
	       wrap_s == GL_CLAMP_TO_EDGE ||
	       wrap_s == GL_CLAMP_TO_BORDER);
	assert(wrap_t == GL_REPEAT ||
	       wrap_t == GL_MIRRORED_REPEAT ||
	       wrap_t == GL_CLAMP_TO_EDGE ||
	       wrap_t == GL_CLAMP_TO_BORDER);
#endif
	assert(mag_filter == GL_NEAREST ||
	       mag_filter == GL_LINEAR);
	assert(min_filter == GL_NEAREST ||
	       min_filter == GL_LINEAR ||
	       min_filter == GL_NEAREST_MIPMAP_NEAREST ||
	       min_filter == GL_NEAREST_MIPMAP_LINEAR ||
	       min_filter == GL_LINEAR_MIPMAP_NEAREST ||
	       min_filter == GL_LINEAR_MIPMAP_LINEAR);
	assert(logfile != "");

	ofstream logstream(logfile.c_str());
	return load(string(a_name), wrap_s, wrap_t, mag_filter, min_filter, logstream);
	// logstream.close() will be called by ofstream destructor
}

unsigned int TextureManager :: load (const char* a_name,
                                     unsigned int wrap_s,
                                     unsigned int wrap_t,
                                     unsigned int mag_filter,
                                     unsigned int min_filter,
                                     std::ostream& r_logstream)
{
	assert(a_name != NULL);
	assert(!isLoaded(a_name));
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	assert(wrap_s == GL_REPEAT || wrap_s == GL_CLAMP);
	assert(wrap_t == GL_REPEAT || wrap_t == GL_CLAMP);
#else
	assert(wrap_s == GL_REPEAT ||
	       wrap_s == GL_MIRRORED_REPEAT ||
	       wrap_s == GL_CLAMP_TO_EDGE ||
	       wrap_s == GL_CLAMP_TO_BORDER);
	assert(wrap_t == GL_REPEAT ||
	       wrap_t == GL_MIRRORED_REPEAT ||
	       wrap_t == GL_CLAMP_TO_EDGE ||
	       wrap_t == GL_CLAMP_TO_BORDER);
#endif
	assert(mag_filter == GL_NEAREST ||
	       mag_filter == GL_LINEAR);
	assert(min_filter == GL_NEAREST ||
	       min_filter == GL_LINEAR ||
	       min_filter == GL_NEAREST_MIPMAP_NEAREST ||
	       min_filter == GL_NEAREST_MIPMAP_LINEAR ||
	       min_filter == GL_LINEAR_MIPMAP_NEAREST ||
	       min_filter == GL_LINEAR_MIPMAP_LINEAR);

	return load(string(a_name), wrap_s, wrap_t, mag_filter, min_filter, r_logstream);
}

unsigned int TextureManager :: load (const std::string& name,
                                     unsigned int wrap_s,
                                     unsigned int wrap_t,
                                     unsigned int mag_filter,
                                     unsigned int min_filter)
{
	assert(!isLoaded(name));
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	assert(wrap_s == GL_REPEAT || wrap_s == GL_CLAMP);
	assert(wrap_t == GL_REPEAT || wrap_t == GL_CLAMP);
#else
	assert(wrap_s == GL_REPEAT ||
	       wrap_s == GL_MIRRORED_REPEAT ||
	       wrap_s == GL_CLAMP_TO_EDGE ||
	       wrap_s == GL_CLAMP_TO_BORDER);
	assert(wrap_t == GL_REPEAT ||
	       wrap_t == GL_MIRRORED_REPEAT ||
	       wrap_t == GL_CLAMP_TO_EDGE ||
	       wrap_t == GL_CLAMP_TO_BORDER);
#endif
	assert(mag_filter == GL_NEAREST ||
	       mag_filter == GL_LINEAR);
	assert(min_filter == GL_NEAREST ||
	       min_filter == GL_LINEAR ||
	       min_filter == GL_NEAREST_MIPMAP_NEAREST ||
	       min_filter == GL_NEAREST_MIPMAP_LINEAR ||
	       min_filter == GL_LINEAR_MIPMAP_NEAREST ||
	       min_filter == GL_LINEAR_MIPMAP_LINEAR);

	return load(name, wrap_s, wrap_t, mag_filter, min_filter, cerr);
}

unsigned int TextureManager :: load (const std::string& name,
                                     unsigned int wrap_s,
                                     unsigned int wrap_t,
                                     unsigned int mag_filter,
                                     unsigned int min_filter,
                                     const std::string& logfile)
{
	assert(!isLoaded(name));
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	assert(wrap_s == GL_REPEAT || wrap_s == GL_CLAMP);
	assert(wrap_t == GL_REPEAT || wrap_t == GL_CLAMP);
#else
	assert(wrap_s == GL_REPEAT ||
	       wrap_s == GL_MIRRORED_REPEAT ||
	       wrap_s == GL_CLAMP_TO_EDGE ||
	       wrap_s == GL_CLAMP_TO_BORDER);
	assert(wrap_t == GL_REPEAT ||
	       wrap_t == GL_MIRRORED_REPEAT ||
	       wrap_t == GL_CLAMP_TO_EDGE ||
	       wrap_t == GL_CLAMP_TO_BORDER);
#endif
	assert(mag_filter == GL_NEAREST ||
	       mag_filter == GL_LINEAR);
	assert(min_filter == GL_NEAREST ||
	       min_filter == GL_LINEAR ||
	       min_filter == GL_NEAREST_MIPMAP_NEAREST ||
	       min_filter == GL_NEAREST_MIPMAP_LINEAR ||
	       min_filter == GL_LINEAR_MIPMAP_NEAREST ||
	       min_filter == GL_LINEAR_MIPMAP_LINEAR);
	assert(logfile != "");

	ofstream logstream(logfile.c_str());
	return load(name, wrap_s, wrap_t, mag_filter, min_filter, logstream);
	// logstream.close() will be called by ofstream destructor
}

unsigned int TextureManager :: load (const std::string& name,
                                     unsigned int wrap_s,
                                     unsigned int wrap_t,
                                     unsigned int mag_filter,
                                     unsigned int min_filter,
                                     std::ostream& r_logstream)
{
	assert(!isLoaded(name));
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	assert(wrap_s == GL_REPEAT || wrap_s == GL_CLAMP);
	assert(wrap_t == GL_REPEAT || wrap_t == GL_CLAMP);
#else
	assert(wrap_s == GL_REPEAT ||
	       wrap_s == GL_MIRRORED_REPEAT ||
	       wrap_s == GL_CLAMP_TO_EDGE ||
	       wrap_s == GL_CLAMP_TO_BORDER);
	assert(wrap_t == GL_REPEAT ||
	       wrap_t == GL_MIRRORED_REPEAT ||
	       wrap_t == GL_CLAMP_TO_EDGE ||
	       wrap_t == GL_CLAMP_TO_BORDER);
#endif
	assert(mag_filter == GL_NEAREST ||
	       mag_filter == GL_LINEAR);
	assert(min_filter == GL_NEAREST ||
	       min_filter == GL_LINEAR ||
	       min_filter == GL_NEAREST_MIPMAP_NEAREST ||
	       min_filter == GL_NEAREST_MIPMAP_LINEAR ||
	       min_filter == GL_LINEAR_MIPMAP_NEAREST ||
	       min_filter == GL_LINEAR_MIPMAP_LINEAR);

	//  THESE FUNCTIONS SHOULD ALLOW DIFFERENT WRAPPING MODES IN DIFFERENT DIRECTIONS  <|>

	//
	//  All gets, activates, and loads without a transparent
	//    colour go through this function.
	//

	string lower = toLowercase(name);
	if(endsWith(lower, ".bmp"))
	{
		TextureBmp texture_bmp(name.c_str(), r_logstream);
		if(texture_bmp.isBad())
		{
			// TextureBmp prints loading error
			return TEXTURE_INDEX_INVALID;
		}
		else
		{
			//
			//  Texture is flipped when loading
			//    Should it be unflipped here?
			//    Standard is not clear
			//    Not flipping here matches Maya
			//
			//  If changing this, also change load function with
			//    transparent colour below.
			//
			//texture_bmp.mirrorY();
			//

			return add(texture_bmp.addToOpenGL(wrap_s, wrap_t, mag_filter, min_filter), name);
		}
	}
	else if(endsWith(lower, ".png"))
	{
#ifdef OBJ_LIBRARY_LOAD_PNG_TEXTURES
		if(wrap_s != wrap_t)
		{
			r_logstream << "Warning: PNG texture must use the same wrapping mode for all" << endl;
			r_logstream << "         directions.  Using S wrapping mode for " << name << endl;
		}

		unsigned int texture_name = pngBind(name.c_str(),
		                                    PNG_BUILDMIPMAPS,
		                                    PNG_ALPHA,
		                                    NULL,
		                                    wrap_s,
		                                    min_filter,
		                                    mag_filter);
		if(texture_name == 0)
		{
			r_logstream << "Error: Loading failed: " << name << endl;
			return TEXTURE_INDEX_INVALID;
		}
		else
			return add(texture_name, name);
#else
		r_logstream << "Error: Loading .png textures is disabled: " << name << endl;
		return TEXTURE_INDEX_INVALID;
#endif
	}
	else
	{
		r_logstream << "Error: Invalid image file extension: " << name << endl;
		return TEXTURE_INDEX_INVALID;
	}
}

unsigned int TextureManager :: load (const char* a_name,
                                     unsigned int wrap_s,
                                     unsigned int wrap_t,
                                     unsigned int mag_filter,
                                     unsigned int min_filter,
                                     const Vector3& transparent_colour)
{
	assert(a_name != NULL);
	assert(!isLoaded(a_name));
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	assert(wrap_s == GL_REPEAT || wrap_s == GL_CLAMP);
	assert(wrap_t == GL_REPEAT || wrap_t == GL_CLAMP);
#else
	assert(wrap_s == GL_REPEAT ||
	       wrap_s == GL_MIRRORED_REPEAT ||
	       wrap_s == GL_CLAMP_TO_EDGE ||
	       wrap_s == GL_CLAMP_TO_BORDER);
	assert(wrap_t == GL_REPEAT ||
	       wrap_t == GL_MIRRORED_REPEAT ||
	       wrap_t == GL_CLAMP_TO_EDGE ||
	       wrap_t == GL_CLAMP_TO_BORDER);
#endif
	assert(mag_filter == GL_NEAREST ||
	       mag_filter == GL_LINEAR);
	assert(min_filter == GL_NEAREST ||
	       min_filter == GL_LINEAR ||
	       min_filter == GL_NEAREST_MIPMAP_NEAREST ||
	       min_filter == GL_NEAREST_MIPMAP_LINEAR ||
	       min_filter == GL_LINEAR_MIPMAP_NEAREST ||
	       min_filter == GL_LINEAR_MIPMAP_LINEAR);
	assert(transparent_colour.isAllComponentsNonNegative());
	assert(transparent_colour.isAllComponentsLessThanOrEqual(1.0));

	return load(string(a_name), wrap_s, wrap_t, mag_filter, min_filter, transparent_colour, cerr);
}

unsigned int TextureManager :: load (const char* a_name,
                                     unsigned int wrap_s,
                                     unsigned int wrap_t,
                                     unsigned int mag_filter,
                                     unsigned int min_filter,
                                     const Vector3& transparent_colour,
                                     const std::string& logfile)
{
	assert(a_name != NULL);
	assert(!isLoaded(a_name));
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	assert(wrap_s == GL_REPEAT || wrap_s == GL_CLAMP);
	assert(wrap_t == GL_REPEAT || wrap_t == GL_CLAMP);
#else
	assert(wrap_s == GL_REPEAT ||
	       wrap_s == GL_MIRRORED_REPEAT ||
	       wrap_s == GL_CLAMP_TO_EDGE ||
	       wrap_s == GL_CLAMP_TO_BORDER);
	assert(wrap_t == GL_REPEAT ||
	       wrap_t == GL_MIRRORED_REPEAT ||
	       wrap_t == GL_CLAMP_TO_EDGE ||
	       wrap_t == GL_CLAMP_TO_BORDER);
#endif
	assert(mag_filter == GL_NEAREST ||
	       mag_filter == GL_LINEAR);
	assert(min_filter == GL_NEAREST ||
	       min_filter == GL_LINEAR ||
	       min_filter == GL_NEAREST_MIPMAP_NEAREST ||
	       min_filter == GL_NEAREST_MIPMAP_LINEAR ||
	       min_filter == GL_LINEAR_MIPMAP_NEAREST ||
	       min_filter == GL_LINEAR_MIPMAP_LINEAR);
	assert(transparent_colour.isAllComponentsNonNegative());
	assert(transparent_colour.isAllComponentsLessThanOrEqual(1.0));
	assert(logfile != "");

	ofstream logstream(logfile.c_str());
	return load(string(a_name), wrap_s, wrap_t, mag_filter, min_filter, transparent_colour, logstream);
	// logstream.close() will be called by ofstream destructor
}

unsigned int TextureManager :: load (const char* a_name,
                                     unsigned int wrap_s,
                                     unsigned int wrap_t,
                                     unsigned int mag_filter,
                                     unsigned int min_filter,
                                     const Vector3& transparent_colour,
                                     std::ostream& r_logstream)
{
	assert(a_name != NULL);
	assert(!isLoaded(a_name));
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	assert(wrap_s == GL_REPEAT || wrap_s == GL_CLAMP);
	assert(wrap_t == GL_REPEAT || wrap_t == GL_CLAMP);
#else
	assert(wrap_s == GL_REPEAT ||
	       wrap_s == GL_MIRRORED_REPEAT ||
	       wrap_s == GL_CLAMP_TO_EDGE ||
	       wrap_s == GL_CLAMP_TO_BORDER);
	assert(wrap_t == GL_REPEAT ||
	       wrap_t == GL_MIRRORED_REPEAT ||
	       wrap_t == GL_CLAMP_TO_EDGE ||
	       wrap_t == GL_CLAMP_TO_BORDER);
#endif
	assert(mag_filter == GL_NEAREST ||
	       mag_filter == GL_LINEAR);
	assert(min_filter == GL_NEAREST ||
	       min_filter == GL_LINEAR ||
	       min_filter == GL_NEAREST_MIPMAP_NEAREST ||
	       min_filter == GL_NEAREST_MIPMAP_LINEAR ||
	       min_filter == GL_LINEAR_MIPMAP_NEAREST ||
	       min_filter == GL_LINEAR_MIPMAP_LINEAR);
	assert(transparent_colour.isAllComponentsNonNegative());
	assert(transparent_colour.isAllComponentsLessThanOrEqual(1.0));

	return load(string(a_name), wrap_s, wrap_t, mag_filter, min_filter, transparent_colour, r_logstream);
}

unsigned int TextureManager :: load (const std::string& name,
                                     unsigned int wrap_s,
                                     unsigned int wrap_t,
                                     unsigned int mag_filter,
                                     unsigned int min_filter,
                                     const Vector3& transparent_colour)
{
	assert(!isLoaded(name));
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	assert(wrap_s == GL_REPEAT || wrap_s == GL_CLAMP);
	assert(wrap_t == GL_REPEAT || wrap_t == GL_CLAMP);
#else
	assert(wrap_s == GL_REPEAT ||
	       wrap_s == GL_MIRRORED_REPEAT ||
	       wrap_s == GL_CLAMP_TO_EDGE ||
	       wrap_s == GL_CLAMP_TO_BORDER);
	assert(wrap_t == GL_REPEAT ||
	       wrap_t == GL_MIRRORED_REPEAT ||
	       wrap_t == GL_CLAMP_TO_EDGE ||
	       wrap_t == GL_CLAMP_TO_BORDER);
#endif
	assert(mag_filter == GL_NEAREST ||
	       mag_filter == GL_LINEAR);
	assert(min_filter == GL_NEAREST ||
	       min_filter == GL_LINEAR ||
	       min_filter == GL_NEAREST_MIPMAP_NEAREST ||
	       min_filter == GL_NEAREST_MIPMAP_LINEAR ||
	       min_filter == GL_LINEAR_MIPMAP_NEAREST ||
	       min_filter == GL_LINEAR_MIPMAP_LINEAR);
	assert(transparent_colour.isAllComponentsNonNegative());
	assert(transparent_colour.isAllComponentsLessThanOrEqual(1.0));

	return load(name, wrap_s, wrap_t, mag_filter, min_filter, transparent_colour, cerr);
}

unsigned int TextureManager :: load (const std::string& name,
                                     unsigned int wrap_s,
                                     unsigned int wrap_t,
                                     unsigned int mag_filter,
                                     unsigned int min_filter,
                                     const Vector3& transparent_colour,
                                     const std::string& logfile)
{
	assert(!isLoaded(name));
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	assert(wrap_s == GL_REPEAT || wrap_s == GL_CLAMP);
	assert(wrap_t == GL_REPEAT || wrap_t == GL_CLAMP);
#else
	assert(wrap_s == GL_REPEAT ||
	       wrap_s == GL_MIRRORED_REPEAT ||
	       wrap_s == GL_CLAMP_TO_EDGE ||
	       wrap_s == GL_CLAMP_TO_BORDER);
	assert(wrap_t == GL_REPEAT ||
	       wrap_t == GL_MIRRORED_REPEAT ||
	       wrap_t == GL_CLAMP_TO_EDGE ||
	       wrap_t == GL_CLAMP_TO_BORDER);
#endif
	assert(mag_filter == GL_NEAREST ||
	       mag_filter == GL_LINEAR);
	assert(min_filter == GL_NEAREST ||
	       min_filter == GL_LINEAR ||
	       min_filter == GL_NEAREST_MIPMAP_NEAREST ||
	       min_filter == GL_NEAREST_MIPMAP_LINEAR ||
	       min_filter == GL_LINEAR_MIPMAP_NEAREST ||
	       min_filter == GL_LINEAR_MIPMAP_LINEAR);
	assert(transparent_colour.isAllComponentsNonNegative());
	assert(transparent_colour.isAllComponentsLessThanOrEqual(1.0));
	assert(logfile != "");

	ofstream logstream(logfile.c_str());
	return load(name, wrap_s, wrap_t, mag_filter, min_filter, transparent_colour, logstream);
	// logstream.close() will be called by ofstream destructor
}

unsigned int TextureManager :: load (const std::string& name,
                                     unsigned int wrap_s,
                                     unsigned int wrap_t,
                                     unsigned int mag_filter,
                                     unsigned int min_filter,
                                     const Vector3& transparent_colour,
                                     std::ostream& r_logstream)
{
	assert(!isLoaded(name));
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	assert(wrap_s == GL_REPEAT || wrap_s == GL_CLAMP);
	assert(wrap_t == GL_REPEAT || wrap_t == GL_CLAMP);
#else
	assert(wrap_s == GL_REPEAT ||
	       wrap_s == GL_MIRRORED_REPEAT ||
	       wrap_s == GL_CLAMP_TO_EDGE ||
	       wrap_s == GL_CLAMP_TO_BORDER);
	assert(wrap_t == GL_REPEAT ||
	       wrap_t == GL_MIRRORED_REPEAT ||
	       wrap_t == GL_CLAMP_TO_EDGE ||
	       wrap_t == GL_CLAMP_TO_BORDER);
#endif
	assert(mag_filter == GL_NEAREST ||
	       mag_filter == GL_LINEAR);
	assert(min_filter == GL_NEAREST ||
	       min_filter == GL_LINEAR ||
	       min_filter == GL_NEAREST_MIPMAP_NEAREST ||
	       min_filter == GL_NEAREST_MIPMAP_LINEAR ||
	       min_filter == GL_LINEAR_MIPMAP_NEAREST ||
	       min_filter == GL_LINEAR_MIPMAP_LINEAR);
	assert(transparent_colour.isAllComponentsNonNegative());
	assert(transparent_colour.isAllComponentsLessThanOrEqual(1.0));

	//
	//  All gets, activates, and loads with a transparent colour
	//    go through this function.
	//

	// calculate transparent RGB vlaues
	Vector3 transparent_255 = transparent_colour * 255.0;
	assert(transparent_255.isAllComponentsNonNegative());
	assert(transparent_255.isAllComponentsLessThanOrEqual(255.0));
	assert((int)(transparent_255.x) >= 0x00);
	assert((int)(transparent_255.y) >= 0x00);
	assert((int)(transparent_255.z) >= 0x00);
	assert((int)(transparent_255.x) <= 0xFF);
	assert((int)(transparent_255.y) <= 0xFF);
	assert((int)(transparent_255.z) <= 0xFF);
	g_transparent_red   = (unsigned char)(transparent_255.x);
	g_transparent_green = (unsigned char)(transparent_255.y);
	g_transparent_blue  = (unsigned char)(transparent_255.z);

	// load the texture
	string lower = toLowercase(name);
	if(endsWith(lower, ".bmp"))
	{
		TextureBmp texture_bmp(name.c_str(), r_logstream);
		if(texture_bmp.isBad())
			return TEXTURE_INDEX_INVALID;
		else
		{
			// if texture is Y-mirrored above, also Y-mirror here

			TextureBmp texture_alpha(texture_bmp,
			                         0, 0, texture_bmp.getWidth(), texture_bmp.getHeight(),
			                         g_transparent_red, g_transparent_green, g_transparent_blue);
			return add(texture_alpha.addToOpenGL(wrap_s, wrap_t, mag_filter, min_filter), name);
		}
	}
	else if(endsWith(lower, ".png"))
	{
#ifdef OBJ_LIBRARY_LOAD_PNG_TEXTURES
		if(wrap_s != wrap_t)
		{
			r_logstream << "Warning: PNG texture must use the same wrapping mode for all" << endl;
			r_logstream << "         directions.  Using S wrapping mode for " << name << endl;
		}

		pngSetAlphaCallback(&pngAlphaStencilCallback);
		unsigned int texture_name = pngBind(name.c_str(),
		                                    PNG_BUILDMIPMAPS,
		                                    PNG_CALLBACK,
		                                    NULL,
		                                    wrap_s,
		                                    min_filter,
		                                    mag_filter);
		if(texture_name == 0)
		{
			r_logstream << "Error: Loading failed: " << name << endl;
			return TEXTURE_INDEX_INVALID;
		}
		else
			return add(texture_name, name);
#else
		r_logstream << "Error: Loading .png textures is disabled: " << name << endl;
		return TEXTURE_INDEX_INVALID;
#endif
	}
	else
	{
		r_logstream << "Error: Invalid image file extension: " << name << endl;
		return TEXTURE_INDEX_INVALID;
	}
}



void TextureManager :: unloadAll ()
{
	for(unsigned int i = 0; i < gvp_textures.size(); i++)
	{
		assert(gvp_textures[i] != NULL);
		delete gvp_textures[i];	// destructor frees video memory
	}
	gvp_textures.clear();
}


