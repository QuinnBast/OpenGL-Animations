//
//  SpriteFont.cpp
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
//  This file can interact with shaders:  UPDATE THIS  <|>
//    -> SpriteFont::drawLineOfText (C++ code)
//       -> draws 1 point per character
//    -> SpriteFont.vert (vertex shader)
//    -> SpriteFont.geom (geometry shader)
//      -> builds a 2-trinagle triangle strip for each point
//      -> calculates texture coordinates
//    -> SpriteFont.frag (fragment shader)
//      -> Draws the text based on texture coordinates
//

#include <cassert>
#include <cctype>
#include <string>
#include <vector>

#include "ObjSettings.h"

#ifdef OBJ_LIBRARY_SHADER_DISPLAY
	#ifndef OBJ_LIBRARY_GLM_INTERACTION
		#error "ObjShader can only be used with shaders if OBJ_LIBRARY_GLM_INTERACTION is #defined"
	#endif
	#include "../glm/glm.hpp"
	#include "../GetGlutWithShaders.h"
	#include "ObjVbo.h"
	#include "ObjVao.h"
	#include "ObjShader.h"
#else
	#include "../GetGlut.h"
#endif

#include "TextureBmp.h"
#include "SpriteFont.h"



using namespace std;
using namespace ObjLibrary;
namespace
{
	const unsigned int TEXTURES_PER_ROW = 16;

	const unsigned char TAB_SPACING_CHARACTER  = ' ';
	const unsigned int  TAB_SPACING_MULTIPLIER = 8;

	// This must match the constant in ObjShaderSpriteFont.geom
	const unsigned int         SLANT_MASK = 0xC;

	const unsigned int         WIDTH_MASK = 0x30;
	const unsigned int     UNDERLINE_MASK = 0xC0;
	const unsigned int STRIKETHROUGH_MASK = 0x300;

	const unsigned int INVALID_FORMAT_MASK = ~(0x3FFu);

	//
	//  getExtraWidthForFormat
	//  getExtraHeightForFormat
	//
	//  Purpose: To determine the change in width/height
	//           needed for the specified format.
	//  Parameter(s):
	//    <1> format: The format
	//  Precondition(s): N/A
	//  Returns: The change in width/height needed to
	//           display a string with formatting format.
	//  Side Effect: N/A
	//
	int getExtraWidthForFormat (unsigned int format)
	{
		int extra = 0;

		if((format & SpriteFont::BOLD) == SpriteFont::BOLD)
			extra += 1;

		switch(format & WIDTH_MASK)
		{
		case SpriteFont::WIDE:      extra += 1; break;
		case SpriteFont::VERY_WIDE: extra += 3; break;
		case SpriteFont::NARROW:    extra -= 1; break;
		}

		return extra;
	}

	int getExtraHeightForFormat (unsigned int format)
	{
#ifdef OBJ_LIBRARY_SHADER_DISPLAY
		// underlines are not supported with shaders
		return 0;
#else
		switch(format & UNDERLINE_MASK)
		{
		case SpriteFont::UNDERLINE:        return 2;
		case SpriteFont::DOUBLE_UNDERLINE: return 3;
		case SpriteFont::RED_UNDERLINE:    return 2;
		default:                           return 0;
		}
#endif
	}

	//
	//  getSlantAmountForFormat
	//
	//  Purpose: To determine the slant amount needed for
	//           the specified format.  This is a movement
	//           in the +X direction on the top and the -X
	//           on the bottom.
	//  Parameter(s):
	//    <1> format: The format
	//    <2> height: The height of the text to be slanted
	//  Precondition(s): N/A
	//  Returns: The slant amount needed to display a string
	//           with formatting format.
	//  Side Effect: N/A
	//
	// This function should match the one in
	//    ObjShaderSpriteFont.geom.
	//
	int getSlantAmountForFormat (unsigned int format, int height)
	{
		switch(format & SLANT_MASK)
		{
		case SpriteFont::ITALICS:      return height / 6;
		case SpriteFont::SLANT:        return height / 3;
		case SpriteFont::BACK_ITALICS: return height / -6;
		default:                       return 0;
		}
	}

#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	//
	//  drawLineThrough
	//
	//  Purpose: To draw an underline or strikethrough for
	//           text of the specified length.  This is the
	//           same line, it just is at differant heights.
	//  Parameter(s):
	//    <1> start_x
	//    <2> end_x: The start and end of the text
	//    <3> y: The y-coordinate of the line
	//    <4> depth: The depth of the line
	//    <5> variant: The variant of line through to draw
	//  Precondition(s):
	//    <1> start_x <= end_x
	//    <2> depth >= 0.0
	//    <3> depth <= 1.0
	//  Returns: N/A
	//  Side Effect: An underline or strikethrough is drawn
	//               with y-coordinate y for text between
	//               the x-coordinates start_x amd end_x.
	//               Variants UNDERLINE and STRIKETHROUGH
	//               draw a single line, DOUBLE_UNDERLINE
	//               and DOUBLE_STRIKETHROUGH draw a double
	//               line, and RED_UNDERLINE and
	//               RED_STRIKETHROUGH draw a single thick
	//               red line.
	//
	void drawLineThrough (double start_x,
	                      double end_x,
	                      double y,
	                      double depth,
	                      unsigned int format)
	{
		assert(start_x <= end_x);
		assert(depth >= 0.0);
		assert(depth <= 1.0);

		switch(format)
		{
		case SpriteFont::UNDERLINE:
		case SpriteFont::STRIKETHROUGH:
			glDisable(GL_TEXTURE_2D);
			glBegin(GL_LINES);
				glVertex3d(start_x - 1, y, depth);
				glVertex3d(end_x   + 2, y, depth);
			glEnd();
			break;
		case SpriteFont::DOUBLE_UNDERLINE:
		case SpriteFont::DOUBLE_STRIKETHROUGH:
			glDisable(GL_TEXTURE_2D);
			glBegin(GL_LINES);
				glVertex3d(start_x - 1, y - 1, depth);
				glVertex3d(end_x   + 2, y - 1, depth);
				glVertex3d(start_x - 1, y + 1, depth);
				glVertex3d(end_x   + 2, y + 1, depth);
			glEnd();
			break;
		case SpriteFont::RED_UNDERLINE:
		case SpriteFont::RED_STRIKETHROUGH:
			glDisable(GL_TEXTURE_2D);
			glColor4ub(0xFF, 0x00, 0x00, 0xFF);
			glBegin(GL_LINES);
				glVertex3d(start_x - 1, y - 1, depth);
				glVertex3d(end_x   + 2, y - 1, depth);
				glVertex3d(start_x - 1, y, depth);
				glVertex3d(end_x   + 2, y, depth);
			glEnd();
			break;
		}
	}
#endif



	bool g_is_2d_view_set_up = false;


#ifdef OBJ_LIBRARY_SHADER_DISPLAY
	// variables and functions for use with shaders

	glm::vec2 g_2d_view_size;

	ObjVao g_vao;
	ObjVbo<float> g_vbo_position;
	ObjVbo<int>   g_vbo_character;

	const unsigned int BLOCK_SIZE_MAX = 0x1000;

	const unsigned int POSITION_COMPONENT_COUNT = 2;
	const unsigned int POSITION_ELEMENT_COUNT   = BLOCK_SIZE_MAX * POSITION_COMPONENT_COUNT;

	const unsigned int CHARACTER_COMPONENT_COUNT = 1;
	const unsigned int CHARACTER_ELEMENT_COUNT   = BLOCK_SIZE_MAX * CHARACTER_COMPONENT_COUNT;



	//
	//  initVao
	//
	//  Purpose: To inititalize the VAO for sprite fonts and its
	//           components.
	//  Parameter(s): N/A
	//  Precondition(s):
	//    <1> g_vao.isEmpty()
	//  Returns: N/A
	//  Side Effect: The VAO and VBOs for sprite fonts are
	//               loaded.
	//
	void initVao ()
	{
		assert(g_vao.isEmpty());

		// init VBOs with default values

		glm::vec2 a_position [ POSITION_ELEMENT_COUNT];
		int       a_character[CHARACTER_ELEMENT_COUNT];

		g_vbo_position.init(GL_ARRAY_BUFFER,
		                    POSITION_ELEMENT_COUNT,
		                    &(a_position[0].x),
		                    GL_STREAM_DRAW);

		g_vbo_character.init(GL_ARRAY_BUFFER,
		                     CHARACTER_ELEMENT_COUNT,
		                     a_character,
		                     GL_STREAM_DRAW);

		// initialize VAO to use (changable) VBO

		g_vao.init();
		g_vao.bind();

		// attach position data
		g_vbo_position.bind();
		//  glVertexAttribPointer(slot index,
		//                        how many components per vertex,
		//                        component type,
		//                        always GL_FALSE,
		//                        stride in bytes (0 for tightly packed),
		//                        pointer offset typcast to void pointer);
		glVertexAttribPointer(0, POSITION_COMPONENT_COUNT, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		// attach characters to draw
		g_vbo_character.bind();
		//  glVertexAttribIPointer(slot index,
		//                         how many components per vertex,
		//                         component type,
		//                         stride in bytes (0 for tightly packed),
		//                         pointer offset typcast to void pointer);
		glVertexAttribIPointer(1, CHARACTER_COMPONENT_COUNT, GL_UNSIGNED_INT, 0, 0);
		glEnableVertexAttribArray(1);

		// finished
		g_vao.bindNone();  // we don't want to accidentally change it elsewhere

		assert(!g_vao.isEmpty());
	}

#endif



}	// end of anonymous namespace





const unsigned int SpriteFont :: NO_BREAK_NEEDED = ~0u;

bool SpriteFont :: isAPowerOf2 (unsigned int n)
{
	// not that efficent, but relatively simple
	for(unsigned int i = 1; i != 0; i <<= 1)
		if(n == i)
			return true;
	return false;
}

bool SpriteFont :: isValidFormat (unsigned int format)
{
	if((format & INVALID_FORMAT_MASK) == 0)
		return true;
	else
		return false;
}

bool SpriteFont :: is2dViewSetUp ()
{
	return g_is_2d_view_set_up;
}

void SpriteFont :: setUp2dView (int width, int height)
{
	assert(!is2dViewSetUp());

	setUp2dView(width, height, false);

	assert(is2dViewSetUp());
}

void SpriteFont :: setUp2dView (int width, int height,
                                bool is_clear_depth)
{
	assert(!is2dViewSetUp());

	if(width < 1)
		width = 1;
	if(height < 1)
		height = 1;

#ifdef OBJ_LIBRARY_SHADER_DISPLAY
	g_2d_view_size.x = (float)(width);
	g_2d_view_size.y = (float)(height);
#else
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	// glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far);
	glOrtho(0.0, width, 0.0, height, 0.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslated(0.0, (double)(height), 0.0);
	glScaled(1.0, -1.0, 1.0);
#endif

	if(is_clear_depth)
		glClear(GL_DEPTH_BUFFER_BIT);

	g_is_2d_view_set_up = true;
}

void SpriteFont :: unsetUp2dView ()
{
	assert(is2dViewSetUp());

#ifdef OBJ_LIBRARY_SHADER_DISPLAY
	// nothing to do here
#else
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
#endif

	g_is_2d_view_set_up = false;
}



SpriteFont :: SpriteFont ()
		: m_character_count(0),
		  m_image_size(0),
		  m_character_height(0)
{
#ifdef OBJ_LIBRARY_SHADER_DISPLAY
	m_texture_name = 0;
#else
	for(unsigned int i = 0; i < CHARACTER_COUNT_MAX; i++)
		ma_character_name[i] = 0;
#endif

	for(unsigned int i = 0; i < CHARACTER_COUNT_MAX; i++)
		ma_character_width[i] = 0;

	assert(invariant());
}

SpriteFont :: SpriteFont (const char* a_image,
                          unsigned char red,
                          unsigned char green,
                          unsigned char blue)
		: m_character_count(0),
		  m_image_size(0),
		  m_character_height(0)
{
	assert(a_image != NULL);
	assert(red != green || red != blue);

	load(a_image, red, green, blue);

	assert(invariant());
}

SpriteFont :: SpriteFont (const string& image,
                          unsigned char red,
                          unsigned char green,
                          unsigned char blue)
		: m_character_count(0),
		  m_image_size(0),
		  m_character_height(0)
{
	assert(red != green || red != blue);

	load(image.c_str(), red, green, blue);

	assert(invariant());
}

SpriteFont :: ~SpriteFont ()
{
#ifdef OBJ_LIBRARY_SHADER_DISPLAY
	if(m_texture_name != 0)
	{
		// delete the one texture
		glDeleteTextures(1, &m_texture_name);
	}
#else
	if(ma_character_name != 0)
	{
		// delete a whole array of textures - with one command!

		// glDeleteTextures(GLsizei n, const GLuint *textureNames);
		glDeleteTextures(m_character_count, ma_character_name);
	}
#endif
}



bool SpriteFont :: isInitalized () const
{
	if(m_character_count != 0)
		return true;
	else
		return false;
}

bool SpriteFont :: is8Bit () const
{
	assert(isInitalized());

	if(m_character_count == 256)
		return true;
	else
		return false;
}

int SpriteFont :: getHeight () const
{
	assert(isInitalized());

	return m_character_height;
}

int SpriteFont :: getHeight (unsigned int format) const
{
	assert(isInitalized());
	assert(isValidFormat(format));

	return m_character_height + getExtraHeightForFormat(format);
}

int SpriteFont :: getWidth (char character) const
{
	assert(isInitalized());

	return ma_character_width[(unsigned char)(character)];
}

int SpriteFont :: getWidth (char character, unsigned int format) const
{
	assert(isInitalized());
	assert(isValidFormat(format));

	if(character == '\t')
		return getTabWidth();  // letter spacing doesn't affect tab widths
	else
		return ma_character_width[(unsigned char)(character)] + getExtraWidthForFormat(format);
}

int SpriteFont :: getTabWidth () const
{
	assert(isInitalized());

	return ma_character_width['\t'];
}

int SpriteFont :: getTabWidthDefault () const
{
	assert(isInitalized());

	return (int)(ma_character_width[TAB_SPACING_CHARACTER] * TAB_SPACING_MULTIPLIER);
}



int SpriteFont :: getWidth (const char* a_str) const
{
	assert(isInitalized());
	assert(a_str != NULL);

	return getWidth(string(a_str), PLAIN);
}

int SpriteFont :: getWidth (const string& str) const
{
	assert(isInitalized());

	return getWidth(str, PLAIN);
}

int SpriteFont :: getWidth (const char* a_str, unsigned int format) const
{
	assert(isInitalized());
	assert(isValidFormat(format));
	assert(a_str != NULL);

	return getWidth(string(a_str), format);
}

int SpriteFont :: getWidth (const string& str, unsigned int format) const
{
	assert(isInitalized());
	assert(isValidFormat(format));

	int extra_width = getExtraWidthForFormat(format);

	int total_largest = 0;
	int total_current = 0;

	for(unsigned int i = 0; i < str.size(); i++)  
	{
		unsigned char character = str[i];
		int character_width = ma_character_width[character];

		if(character == '\n')
		{
			if(total_largest < total_current)
				total_largest = total_current;
			total_current = 0;
		}
		else if(character == '\t')
		{
			assert(character_width > 0);
			total_current = (1 + total_current / character_width) * character_width;
		}
		else
			total_current += character_width + extra_width;
	}

	if(total_largest < total_current)
		total_largest = total_current;

	return total_largest;
}



unsigned int SpriteFont :: getBreakPoint (const char* a_str,
                                          unsigned int width) const
{
	assert(isInitalized());
	assert(a_str != NULL);

	return getBreakPoint(string(a_str), width, PLAIN);
}

unsigned int SpriteFont :: getBreakPoint (const string& str,
                                          unsigned int width) const
{
	assert(isInitalized());

	return getBreakPoint(str, width, PLAIN);
}

unsigned int SpriteFont :: getBreakPoint (const char* a_str,
                                          unsigned int width,
                                          unsigned int format) const
{
	assert(isInitalized());
	assert(isValidFormat(format));
	assert(a_str != NULL);

	return getBreakPoint(string(a_str), width, format);
}

unsigned int SpriteFont :: getBreakPoint (const string& str,
                                          unsigned int width,
                                          unsigned int format) const
{
	assert(isInitalized());
	assert(isValidFormat(format));

	int extra_width = getExtraWidthForFormat(format);

	unsigned int word_start = 0;
	int total_length = 0;
	for(unsigned int current = 0; current < str.size(); current++)
	{
		unsigned char character = str[current];
		int character_width = ma_character_width[character];

		if(character == '\t')
		{
			assert(character_width > 0);
			total_length = (1 + total_length / character_width) * character_width;
		}
		else if(character != '\n')
			total_length += character_width + extra_width;

		if(current >= 1 && !isspace(character))
		{
			if(isspace(str[current - 1]))
				word_start = current;

			if(total_length > (int)width)
			{
				if(word_start == 0)
					return current;
				else
					return word_start;
			}
		}
		else if(character == '\n')
		{
			if(current + 1 == str.size())
				return NO_BREAK_NEEDED;
			else
				return current + 1;
		}
	}

	return NO_BREAK_NEEDED;
}

vector<string> SpriteFont :: breakString (const char* a_str,
                                          unsigned int width) const
{
	assert(isInitalized());
	assert(a_str != NULL);

	return breakString(string(a_str), width, PLAIN);
}

vector<string> SpriteFont :: breakString (const string& str,
                                          unsigned int width) const
{
	assert(isInitalized());

	return breakString(str, width, PLAIN);
}

vector<string> SpriteFont :: breakString (const char* a_str,
                                          unsigned int width,
                                          unsigned int format) const
{
	assert(isInitalized());
	assert(isValidFormat(format));
	assert(a_str != NULL);

	return breakString(string(a_str), width, format);
}

vector<string> SpriteFont :: breakString (const string& str,
                                          unsigned int width,
                                          unsigned int format) const
{
	assert(isInitalized());
	assert(isValidFormat(format));

	vector<string> results;
	string current = str;

	while(true)	// loop drops out below...
	{
		unsigned int break_point = getBreakPoint(current, width, format);

		if(break_point == SpriteFont::NO_BREAK_NEEDED)
		{
			results.push_back(current);
			return results;	// end of loop
		}
		else
		{
			results.push_back(current.substr(0, break_point));
			current = current.substr(break_point);
		}
	}
}



void SpriteFont :: draw (const char* a_str,
                         double x,
                         double y) const
{
	assert(isInitalized());
	assert(a_str != NULL);

	draw(string(a_str), x, y, 0.0, 0xFF, 0xFF, 0xFF, 0xFF, PLAIN);
}

void SpriteFont :: draw (const string& str,
                         double x,
                         double y) const
{
	assert(isInitalized());

	draw(str, x, y, 0.0, 0xFF, 0xFF, 0xFF, 0xFF, PLAIN);
}

void SpriteFont :: draw (const char* a_str,
                         double x,
                         double y,
                         unsigned int format) const
{
	assert(isInitalized());
	assert(isValidFormat(format));
	assert(a_str != NULL);

	draw(string(a_str), x, y, 0.0, 0xFF, 0xFF, 0xFF, 0xFF, format);
}

void SpriteFont :: draw (const string& str,
                         double x,
                         double y,
                         unsigned int format) const
{
	assert(isInitalized());
	assert(isValidFormat(format));

	draw(str, x, y, 0.0, 0xFF, 0xFF, 0xFF, 0xFF, format);
}

void SpriteFont :: draw (const char* a_str,
                         double x,
                         double y,
                         unsigned char red,
                         unsigned char green,
                         unsigned char blue) const
{
	assert(isInitalized());
	assert(a_str != NULL);

	draw(string(a_str), x, y, 0.0, red, green, blue, 0xFF, PLAIN);
}

void SpriteFont :: draw (const string& str,
                         double x,
                         double y,
                         unsigned char red,
                         unsigned char green,
                         unsigned char blue) const
{
	assert(isInitalized());

	draw(str, x, y, 0.0, red, green, blue, 0xFF, PLAIN);
}

void SpriteFont :: draw (const char* a_str,
                         double x,
                         double y,
                         unsigned char red,
                         unsigned char green,
                         unsigned char blue,
                         unsigned int format) const
{
	assert(isInitalized());
	assert(isValidFormat(format));
	assert(a_str != NULL);

	draw(string(a_str), x, y, 0.0, red, green, blue, 0xFF, format);
}

void SpriteFont :: draw (const string& str,
                         double x,
                         double y,
                         unsigned char red,
                         unsigned char green,
                         unsigned char blue,
                         unsigned int format) const
{
	assert(isInitalized());
	assert(isValidFormat(format));

	draw(str, x, y, 0.0, red, green, blue, 0xFF, format);
}

void SpriteFont :: draw (const char* a_str,
                         double x,
                         double y,
                         unsigned char red,
                         unsigned char green,
                         unsigned char blue,
                         unsigned char alpha,
                         unsigned int format) const
{
	assert(isInitalized());
	assert(isValidFormat(format));
	assert(a_str != NULL);

	draw(string(a_str), x, y, 0.0, red, green, blue, alpha, format);
}

void SpriteFont :: draw (const string& str,
                         double x,
                         double y,
                         unsigned char red,
                         unsigned char green,
                         unsigned char blue,
                         unsigned char alpha,
                         unsigned int format) const
{
	assert(isInitalized());
	assert(isValidFormat(format));

	draw(str, x, y, 0.0, red, green, blue, alpha, format);
}

void SpriteFont :: draw (const char* a_str,
                         double x,
                         double y,
                         double depth,
                         unsigned char red,
                         unsigned char green,
                         unsigned char blue,
                         unsigned char alpha,
                         unsigned int format) const
{
	assert(isInitalized());
	assert(isValidFormat(format));
	assert(a_str != NULL);

	draw(string(a_str), x, y, depth, red, green, blue, alpha, format);
}

void SpriteFont :: draw (const string& str,
                         double x,
                         double y,
                         double depth,
                         unsigned char red,
                         unsigned char green,
                         unsigned char blue,
                         unsigned char alpha,
                         unsigned int format) const
{
	assert(isInitalized());
	assert(isValidFormat(format));

	setUpForDrawing(depth, red, green, blue, alpha, format);
	drawLineOfText(str, x, y, depth, format);
	unsetUpForDrawing();
}

void SpriteFont :: draw (const vector<string>& lines,
                         double x,
                         double y) const
{
	assert(isInitalized());

	draw(lines, x, y, 0.0, 0xFF, 0xFF, 0xFF, 0xFF, PLAIN);
}

void SpriteFont :: draw (const vector<string>& lines,
                         double x,
                         double y,
                         unsigned int format) const
{
	assert(isInitalized());
	assert(isValidFormat(format));

	draw(lines, x, y, 0.0, 0xFF, 0xFF, 0xFF, 0xFF, format);
}

void SpriteFont :: draw (const vector<string>& lines,
                         double x,
                         double y,
                         unsigned char red,
                         unsigned char green,
                         unsigned char blue) const
{
	assert(isInitalized());

	draw(lines, x, y, 0.0, red, green, blue, 0xFF, PLAIN);
}

void SpriteFont :: draw (const vector<string>& lines,
                         double x,
                         double y,
                         unsigned char red,
                         unsigned char green,
                         unsigned char blue,
                         unsigned int format) const
{
	assert(isInitalized());
	assert(isValidFormat(format));

	draw(lines, x, y, 0.0, red, green, blue, 0xFF, format);
}

void SpriteFont :: draw (const vector<string>& lines,
                         double x,
                         double y,
                         unsigned char red,
                         unsigned char green,
                         unsigned char blue,
                         unsigned char alpha,
                         unsigned int format) const
{
	assert(isInitalized());
	assert(isValidFormat(format));

	draw(lines, x, y, 0.0, red, green, blue, 0xFF, format);
}

void SpriteFont :: draw (const vector<string>& lines,
                         double x,
                         double y,
                         double depth,
                         unsigned char red,
                         unsigned char green,
                         unsigned char blue,
                         unsigned char alpha,
                         unsigned int format) const
{
	assert(isInitalized());
	assert(depth >= 0.0);
	assert(depth <= 1.0);
	assert(isValidFormat(format));

	int height = getHeight(format);

	setUpForDrawing(depth, red, green, blue, alpha, format);
	for(unsigned int i = 0; i < lines.size(); i++)
	{
		y = drawLineOfText(lines[i], x, y, depth, format);
		if(lines[i].empty() || lines[i].back() != '\n')
			y += height;
	}
	unsetUpForDrawing();
}



void SpriteFont :: load (const char* a_image)
{
	assert(!isInitalized());
	assert(a_image != NULL);

	load(a_image, 0xFF, 0x00, 0xFF);

	assert(invariant());
}

void SpriteFont :: load (const std::string& image)
{
	assert(!isInitalized());

	load(image.c_str(), 0xFF, 0x00, 0xFF);

	assert(invariant());
}

void SpriteFont :: load (const char* a_image,
                         unsigned char red,
                         unsigned char green,
                         unsigned char blue)
{
	assert(!isInitalized());
	assert(a_image != NULL);
	assert(red != green || red != blue);

	static const float A_ZERO[4] = { 0.0f, 0.0f, 0.0f, 0.0f, };

#ifdef OBJ_LIBRARY_SHADER_DISPLAY
	if(g_vao.isEmpty())
		initVao();
	assert(!g_vao.isEmpty());
#endif

	TextureBmp font(a_image);

	assert(font.getWidth() >= 16);
	assert(isAPowerOf2(font.getWidth()));
	assert(font.getHeight() == font.getWidth() || font.getHeight() == font.getWidth() / 2);

	//  Calculate channel that gives best detail.
	//    We will use this if the letters overlap
	//    the outside-coloured area.

	char          channel_to_use;
	unsigned char channel_max;
	if(red <= green && red <= blue)
	{
		channel_to_use = 'r';
		channel_max    = 255 - red;
	}
	else if(green <= blue)
	{
		channel_to_use = 'g';
		channel_max    = 255 - green;
	}
	else
	{
		channel_to_use = 'b';
		channel_max    = 255 - blue;
	}

	if(font.getHeight() == font.getWidth())
		m_character_count = 256;
	else
		m_character_count = 128;
	m_image_size = font.getWidth() / TEXTURES_PER_ROW;

#ifdef OBJ_LIBRARY_SHADER_DISPLAY
	// set up one array texture to hold all characters
	glGenTextures(1, &m_texture_name);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture_name);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// black (i.e. entirely-transparent) border
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// we don't seem to specify a wrapping mode for GL_TEXTURE_WRAP_R (layers)
	// void glTexParameterfv(GLenum target, GLenum pname, const GLfloat * params);
	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, A_ZERO);

	// glTexImage3D(target, level, internalFormat, width, height, depth, border, format, type, data);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED,
	             m_image_size, m_image_size, m_character_count,
	             0, GL_RED, GL_UNSIGNED_BYTE,
	             NULL);
#else
	// set up an array of textures, one for each character
	glGenTextures(m_character_count, ma_character_name);
#endif

	unsigned char* d_tile = new unsigned char[m_image_size * m_image_size];
	for(unsigned int i = 0; i < m_character_count; i++)
	{
		unsigned int cell_x = i % TEXTURES_PER_ROW;
		unsigned int cell_y = i / TEXTURES_PER_ROW;

		unsigned int base_x = cell_x * m_image_size;
		unsigned int base_y = cell_y * m_image_size;

		for(unsigned int y = 0; y < m_image_size; y++)
		{
			unsigned int font_y = base_y + y;

			for(unsigned int x = 0; x < m_image_size; x++)
			{
				unsigned int tile_index = y * m_image_size + x;

				unsigned int font_x = base_x + x;

				unsigned char r = font.getRed(font_x, font_y);
				unsigned char g = font.getGreen(font_x, font_y);
				unsigned char b = font.getBlue(font_x, font_y);

				if(r == red && g == green && b == blue)
				{
					// we are in the "outside" area
					d_tile[tile_index] = 0;
				}
				else if(r != g || r != b)
				{
					//  We are in the "outside" area, but have data.  We
					//    want to use the channel that gives us the most
					//    detailed information
					switch(channel_to_use)
					{
					case 'r': d_tile[tile_index] = ((unsigned int)(r) * 255) / channel_max; break;
					case 'g': d_tile[tile_index] = ((unsigned int)(g) * 255) / channel_max; break;
					case 'b': d_tile[tile_index] = ((unsigned int)(b) * 255) / channel_max; break;
					default:  d_tile[tile_index] = 0; break;
					}
				}
				else
				{
					// a nice ordinary greyscale value
					d_tile[tile_index] = r;
				}
			}
		}

#ifdef OBJ_LIBRARY_SHADER_DISPLAY
		// convert our tile to an OpenGL texture layer

		// glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
		                0, 0, i,
		                m_image_size, m_image_size, 1,
		                GL_RED, GL_UNSIGNED_BYTE,
		                d_tile);
#else
		// convert our tile to an OpenGL texture
		glBindTexture(GL_TEXTURE_2D, ma_character_name[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, m_image_size, m_image_size, 0, GL_ALPHA, GL_UNSIGNED_BYTE, d_tile);
#endif

		// calculate the character width from the first row
		ma_character_width[i] = 0;
		for(unsigned int x2 = 0; x2 <= m_image_size; x2++)
		{
			unsigned int this_x = base_x + x2;

			if(x2 >= m_image_size ||
			   (font.getRed  (this_x, base_y) == red &&
			    font.getGreen(this_x, base_y) == green &&
			    font.getBlue (this_x, base_y) == blue))
			{
				ma_character_width[i] = x2;
				break;
			}
		}
	}
	delete[] d_tile;

	setTabWidthToDefault();

	// calculate the font height from the first column of the first character
	m_character_height = 0;
	for(unsigned int y2 = 0; y2 <= m_image_size; y2++)
	{
		if(y2 >= m_image_size ||
		   (font.getRed  (0, y2) == red &&
		    font.getGreen(0, y2) == green &&
		    font.getBlue (0, y2) == blue))
		{
			m_character_height = y2;
			break;
		}
	}

	// fill in the rest of the arrays with '0's
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	for(unsigned int i2 = m_character_count; i2 < CHARACTER_COUNT_MAX; i2++)
		ma_character_name[i2] = 0;
#endif
	for(unsigned int i2 = m_character_count; i2 < CHARACTER_COUNT_MAX; i2++)
		ma_character_width[i2] = 0;

	assert(invariant());
}

void SpriteFont :: load (const string& image,
                         unsigned char red,
                         unsigned char green,
                         unsigned char blue)
{
	assert(!isInitalized());
	assert(red != green || red != blue);

	load(image.c_str(), red, green, blue);

	assert(invariant());
}

void SpriteFont :: setTabWidthPixels (int width)
{
	assert(isInitalized());
	assert(width > 0);

	ma_character_width['\t'] = width;

	assert(invariant());
}

void SpriteFont :: setTabWidthMultiple (char character, double factor)
{
	assert(isInitalized());
	assert(character != '\n');
	assert(character != '\t');
	assert(factor > 0.0);

	int calculated = (int)(ma_character_width[(unsigned char)(character)] * factor);
	if(calculated == 0)
		setTabWidthPixels(1);
	else
		setTabWidthPixels(calculated);

	assert(invariant());
}

void SpriteFont :: setTabWidthToDefault ()
{
	assert(isInitalized());

	setTabWidthMultiple(TAB_SPACING_CHARACTER, TAB_SPACING_MULTIPLIER);

	assert(invariant());
}




//
//  Copy Constructor
//  Assignment Operator
//
//  These functions have intentionally not been implemented.
//
//  SpriteFont (const SpriteFont& original);
//  SpriteFont& operator= (const SpriteFont& original);
//

void SpriteFont :: setUpForDrawing (double depth,
                                    unsigned char red,
                                    unsigned char green,
                                    unsigned char blue,
                                    unsigned char alpha,
                                    unsigned int format) const
{
	assert(isInitalized());
	assert(isValidFormat(format));

#ifdef OBJ_LIBRARY_SHADER_DISPLAY
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);

	glm::vec3 colour(red / 255.0f, green / 255.0f, blue / 255.0f);
	assert(ObjShader::isLoaded());
	ObjShader::activateSpriteFont(g_2d_view_size, (float)(m_image_size), (float)(depth), colour, format);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture_name);

	assert(!g_vao.isEmpty());
	g_vao.bind();

#else
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_POLYGON_BIT | GL_TEXTURE_BIT | GL_LIGHTING_BIT);
		glDepthFunc(GL_LEQUAL);
		glDisable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);
		glShadeModel(GL_FLAT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.0);
		glEnable(GL_TEXTURE_2D);
		glColor4ub(red, green, blue, alpha);
#endif
}

double SpriteFont :: drawLineOfText (const std::string& str,
                                     double x,
                                     double y,
                                     double depth,
                                     unsigned int format) const
{
	int  extra_width  = getExtraWidthForFormat(format);
	bool is_mirror    = ((format & MIRROR) == MIRROR);

	double end_x    = x + getWidth(str, format);
	int    offset_x = 0;

#ifdef OBJ_LIBRARY_SHADER_DISPLAY
	for(unsigned int block_start = 0; block_start < str.size(); block_start += BLOCK_SIZE_MAX)
	{
		// draw very long strings in smaller blocks of characters
		assert(block_start < str.size());
		unsigned int block_size = str.size() - block_start;
		if(block_size > BLOCK_SIZE_MAX)
			block_size = BLOCK_SIZE_MAX;
		assert(block_size < BLOCK_SIZE_MAX);

		// update VBOs
		glm::vec2 a_position [BLOCK_SIZE_MAX];
		int       a_character[BLOCK_SIZE_MAX];

		unsigned int next_index = 0;
		for(unsigned int i = 0; i < block_size; i++)
		{
			unsigned char character = str[block_start + i];

			if(character == '\n')
			{
				offset_x = 0;
				y += getHeight(format);
			}
			else if(character == '\t')
			{
				int character_width = ma_character_width[character];
				assert(character_width > 0);
				offset_x = (1 + offset_x / character_width) * character_width;
			}
			else
			{
				if(is_mirror)
					a_position[next_index].x = (float)(end_x - offset_x);
				else
					a_position[next_index].x = (float)(    x + offset_x);
				a_position[next_index].y = (float)(y);

				a_character[next_index] = character;

				offset_x += ma_character_width[character] + extra_width;
				next_index++;
			}
		}
		g_vbo_position .update(&(a_position[0].x), 0,  POSITION_COMPONENT_COUNT * next_index);
		g_vbo_character.update(  a_character,      0, CHARACTER_COMPONENT_COUNT * next_index);

		// draw prepared characters
		//  NEED A DEPTH UNIFORM
		glDrawArrays(GL_POINTS, 0, next_index);
	}

#else
	bool is_8bit_font = is8Bit();
	int  slant_amount = getSlantAmountForFormat(format, m_character_height);
	bool is_bold      = ((format & BOLD) == BOLD);

	// we need to start at the right and flip the texture coordinates to draw mirrored text
	double base;
	double left_coord;
	double right_coord;
	if(is_mirror)
	{
		base = end_x;
		left_coord  = 1.0;
		right_coord = 0.0;
	}
	else
	{
		base = x;
		left_coord  = 0.0;
		right_coord = 1.0;
	}

	double bottom = y + m_image_size;

	for(unsigned int i = 0; i < str.size(); i++)
	{
		unsigned char character = str[i];

		// mirrored text is drawn with squares to the left of our curser
		double left;
		if(is_mirror)
			left = base - offset_x - m_image_size;
		else
			left = base + offset_x;
		double right = left + m_image_size;

		bool is_draw = true;
		if(character == '\n')
		{
			offset_x = 0;
			y       += getHeight(format);
			bottom   = y + m_image_size;
		}
		else if(character == '\t')
		{
			int character_width = ma_character_width[character];
			assert(character_width > 0);
			offset_x = (1 + offset_x / character_width) * character_width;
		}
		else if(is_8bit_font || ((character & 0x80) == 0x00))
		{
			// only draw characters we have loaded
		
			glBindTexture(GL_TEXTURE_2D, ma_character_name[character]);
			glBegin(GL_QUADS);
				glTexCoord3d(left_coord,  1.0, depth); glVertex2d(left  - slant_amount, bottom);
				glTexCoord3d(left_coord,  0.0, depth); glVertex2d(left  + slant_amount, y);
				glTexCoord3d(right_coord, 0.0, depth); glVertex2d(right + slant_amount, y);
				glTexCoord3d(right_coord, 1.0, depth); glVertex2d(right - slant_amount, bottom);
			glEnd();

			// bold text is just normal text twice
			if(is_bold)
			{
				glBegin(GL_QUADS);
					glTexCoord3d(left_coord,  1.0, depth); glVertex2d(left  - slant_amount + 1, bottom);
					glTexCoord3d(left_coord,  0.0, depth); glVertex2d(left  + slant_amount + 1, y);
					glTexCoord3d(right_coord, 0.0, depth); glVertex2d(right + slant_amount + 1, y);
					glTexCoord3d(right_coord, 1.0, depth); glVertex2d(right - slant_amount + 1, bottom);
				glEnd();
			}

			offset_x += ma_character_width[character] + extra_width;
		}
	}

	drawLineThrough(x, end_x, y +  m_character_height + 1,      depth, format &     UNDERLINE_MASK);
	drawLineThrough(x, end_x, y + (m_character_height * 2) / 3, depth, format & STRIKETHROUGH_MASK);

#endif

	return y;
}

void SpriteFont :: unsetUpForDrawing () const
{
	assert(isInitalized());

#ifdef OBJ_LIBRARY_SHADER_DISPLAY
	g_vao.bindNone();  // prevent accidental changes
#else
	glPopAttrib();
#endif
}

bool SpriteFont :: invariant () const
{
	if(m_character_count != 0 && m_character_count != 0x80 && m_character_count != 0x100) return false;
	if(m_character_count >  CHARACTER_COUNT_MAX) return false;
	if(m_character_count != 0 && !isAPowerOf2(m_image_size)) return false;
	if(m_character_height > m_image_size) return false;

#ifdef OBJ_LIBRARY_SHADER_DISPLAY
	if(m_character_count != 0 && !glIsTexture(m_texture_name))
		return false;
#endif

	for(unsigned int i = 0; i < m_character_count; i++)
	{
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
		if(!glIsTexture(ma_character_name[i]))
			return false;
#endif
		if(i != '\t' && ma_character_width[i] > m_image_size)
			return false;
	}
	if(m_character_count != 0 && ma_character_width['\t'] <= 0)
		return false;

	return true;
}

