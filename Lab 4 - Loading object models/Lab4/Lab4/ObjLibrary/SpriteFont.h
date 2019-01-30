//
//  SpriteFont.h
//
//  A module to load a sprite font from a bitmap file and use it
//    to display strings with OpenGL graphics.
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

#ifndef OBJ_LIBRARY_SPRITE_FONT_H
#define OBJ_LIBRARY_SPRITE_FONT_H

#include <string>
#include <vector>



namespace ObjLibrary
{



//
//  SpriteFont
//
//  A class to encapsulate a sprite font.  A SpriteFont is
//    created from a bitmap file containing the character images
//    to use.  This image may contain either 8 or 16 rows of 16
//    n x n images, where n is a power of 2.  These indicate
//    128- and 256-character fonts, respectively.  The image
//    should be a greyscale transparency map with black as
//    transparent and white as opaque, but should be stored as a
//    24-bit bitmap.  The font characters may have variable
//    width, up to a square.  The remaining pixels should be
//    filled with a (non-greyscale) colour, which should be
//    specified when the image is loaded.
//
//  Displaying a newline character causes the text to continue
//    at the start of the next line.
//
//  The [TAB] character is handled specially.  It is never
//    displayed and has a width that makes it end at the next
//    tab stop.  Tab stops are placed at regular intervals.  By
//    default, the width of a tab stop is 8 spaces.
//
//  A SpriteFont can be used to print text of any colour.
//    It can only be scaled, but only by applied transformation
//    matrixes (not supported with shaders).  Text can also be
//    printed in a variety of formats, which can be combined
//    with bitwise ORs ('|'s).  At most one option should be
//    selected from each category.
//
//  The legal formatting options are:
//    <-> PLAIN (cannot be combined with any others)
//    <1> BOLD
//        -> Non-shader version looks terrible when combined
//           with transparency
//    <2> MIRROR
//    <3> ITALICS, SLANT, or BACK_ITALICS
//    <4> WIDE, VERY_WIDE, or NARROW
//    <5> UNDERLINE, DOUBLE_UNDERLINE, or RED_UNDERLINE
//        -> Not supported with shaders (has no effect)
//    <6> STRIKETHROUGH, DOUBLE_STRIKETHROUGH, or
//        RED_STRIKETHROUGH
//        -> Not supported with shaders (has no effect)
//
//  Class Invariant:
//    <1> m_character_count == 0 ||
//        m_character_count == 0x80 ||
//        m_character_count == 0x100
//    <2> m_character_count <= CHARACTER_COUNT_MAX
//    <3> m_character_count == 0 ||
//        isAPowerOf2(m_image_size)
//    <4> m_character_height <= m_image_size
#ifdef OBJ_LIBRARY_SHADER_DISPLAY
//    <5> m_character_count == 0 || glIsTexture(m_texture_name)
#else
//    <5> glIsTexture(ma_character_name[i])
//                                FOR 0 <= i < m_character_count
#endif
//    <6> i == '\t' || ma_character_width[i] <= m_image_size
//                                FOR 0 <= i < m_character_count
//    <7> m_character_count == 0 ||
//        ma_character_width['\t'] > 0
//

class SpriteFont
{
public:
//
//  CHARACTER_COUNT_MAX
//
//  The maximum number of characters to store in a SpriteFont.
//
	static const unsigned int CHARACTER_COUNT_MAX = 0x100;

//
//  NO_BREAK_NEEDED
//
//  A constant to indicate that the specified string does not
//    need to be broken to fit on one line.
//
	static const unsigned int NO_BREAK_NEEDED;

//
//  PLAIN
//  BOLD
//  MIRROR
//  ITALICS
//  SLANT
//  BACK_ITALICS
//  WIDE
//  VERY_WIDE
//  NARROW
//  UNDERLINE
//  DOUBLE_UNDERLINE
//  RED_UNDERLINE
//  STRIKETHROUGH
//  DOUBLE_STRIKETHROUGH
//  RED_STRIKETHROUGH
//
//  Constants to represent text formatting options.  These can
//    be combined with bitwise ORs ('|'s).  SLANT is like
//    ITALICS but more so.
//
//  The PLAIN format cannot be combined with any other format.
//
//  The following formats are mutually exclusive:
//    <1> ITALICS, SLANT, and BACK_ITALICS
//    <2> WIDE, NARROW, and VERY_WIDE
//    <3> UNDERLINE, DOUBLE_UNDERLINE, and RED_UNDERLINE
//    <4> STRIKETHROUGH, DOUBLE_STRIKETHROUGH, and
//        RED_STRIKETHROUGH
//
//  These must match the constants in ObjShaderSpriteFont.geom
//    and ObjShaderSpriteFont.frag
//
	static const unsigned int PLAIN                = 0x0;
	static const unsigned int BOLD                 = 0x1;
	static const unsigned int MIRROR               = 0x2;
	static const unsigned int ITALICS              = 0x4;
	static const unsigned int SLANT                = 0x8;
	static const unsigned int BACK_ITALICS         = 0xC;
	static const unsigned int WIDE                 = 0x10;
	static const unsigned int VERY_WIDE            = 0x20;
	static const unsigned int NARROW               = 0x30;
	static const unsigned int UNDERLINE            = 0x40;
	static const unsigned int DOUBLE_UNDERLINE     = 0x80;
	static const unsigned int RED_UNDERLINE        = 0xC0;
	static const unsigned int STRIKETHROUGH        = 0x100;
	static const unsigned int DOUBLE_STRIKETHROUGH = 0x200;
	static const unsigned int RED_STRIKETHROUGH    = 0x300;

//
//  Class Function: isAPowerOf2
//
//  Purpose: To determine if the specified integer is a power
//           of 2.  Remember that 0 is not a power of 2.
//  Parameter(s):
//    <1> n: The integer
//  Precondition(s): N/A
//  Returns: Whether n is a power of two.
//  Side Effect: N/A
//
	static bool isAPowerOf2 (unsigned int n);

//
//  Class Function: isValidFormat
//
//  Purpose: To determine if the specified string format is a
//           valid format to display strings with.
//  Parameter(s):
//    <1> format: The format
//  Precondition(s): N/A
//  Returns: Whether format is a valid format
//  Side Effect: N/A
//
	static bool isValidFormat (unsigned int format);

//
//  Class Function: is2dViewSetUp
//
//  Purpose: To determine of a 2d view has been set up.  This
//           function only detects views set up with the
//           setUp2dView() function.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: Whether a 2d view has been set up.
//  Side Effect: N/A
//
	static bool is2dViewSetUp ();

//
//  Class Function: setUp2dView
//
//  Purpose: To replace the current viewing transforms with
//           those suitable for overlays such as text.  The
//           dimensions of the new viewing area must be
//           specified.  This version of the function does
//           not clear the depth buffer.
//  Parameter(s):
//    <1> width
//    <2> height: The dimensions of the screen
//  Precondition(s):
//    <1> !is2dViewSetUp()
//  Returns: N/A
#ifdef OBJ_LIBRARY_SHADER_DISPLAY
//  Side Effect: If width is less than 1, it is increased to 1.
//               Similarly, If height is less than 1, it is
//               increased to 1.  The view dimensions are stored
//               for use by the draw commands.
#else
//  Side Effect: If width is less than 1, it is increased to 1.
//               Similarly, If height is less than 1, it is
//               increased to 1.  Then the current viewing area
//               is replaced with a 2D view of the new
//               dimensions (width, height).
#endif
//
	static void setUp2dView (int width, int height);

//
//  Class Function: setUp2dView
//
//  Purpose: To replace the current viewing transforms with
//           those suitable for overlays such as text.  The
//           dimensions of the new viewing area must be
//           specified.  Optionally, the depth buffer may be
//           cleared as well.
//  Parameter(s):
//    <1> width
//    <2> height: The dimensions of the screen
//    <3> is_clear_depth: Whether to clear the depth buffer
//  Precondition(s):
//    <1> !is2dViewSetUp()
//  Returns: N/A
#ifdef OBJ_LIBRARY_SHADER_DISPLAY
//  Side Effect: If width is less than 1, it is increased to 1.
//               Similarly, If height is less than 1, it is
//               increased to 1.  The view dimensions are stored
//               for use by the draw commands.  If
//               is_clear_depth is true, the depth buffer is
//               cleared and set to have a range of [0, 1].
//               Otherwise, the depth values become menaingless
//               until unsetUp2dView is called.
#else
//  Side Effect: If width is less than 1, it is increased to 1.
//               Similarly, If height is less than 1, it is
//               increased to 1.  Then the current viewing area
//               is replaced with a 2D view of the new
//               dimensions (width, height).  If is_clear_depth
//               is true, the depth buffer is cleared and set to
//               have a range of [0, 1].  Otherwise, the depth
//               values become menaingless until
//               unsetUp2dView is called.
#endif
//
	static void setUp2dView (int width, int height,
	                         bool is_clear_depth);

//
//  Class Function: unsetUp2dView
//
//  Purpose: To restore the view that was active before the
//           setup2dView() function was called.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> is2dViewSetUp()
//  Returns: N/A
#ifdef OBJ_LIBRARY_SHADER_DISPLAY
//  Side Effect: The 2d view is marked as not set up.  If the
//               depth buffer was cleared, the old depth buffer
//               is not restored and the current depth values
//               become meningless.
#else
//  Side Effect: The old view is restored.
#endif
//
	static void unsetUp2dView ();

public:
//
//  Default Constructor
//
//  Purpose: To create a new SpriteFont with no image specified.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new uninitalized SpriteFont is created.
//
	SpriteFont ();

//
//  Constructor
//
//  Purpose: To create a new SpriteFont with the specifed image.
//  Parameter(s):
//    <1> a_image: The file name of the font image
//    <2> red
//    <3> green
//    <4> blue: The colour indicating the width of the
//              characters in the font
//  Precondition(s):
//    <1> a_image != NULL
//    <2> File a_image exists
//    <3> File a_image is a valid 24-bit bitmap file
//    <4> The width of a_image is at least 16
//    <5> The width of a_image is a power of 2
//    <6> The height of a_image is equal to or half of the width
//        of a_image
//    <7> red != green || red != blue
//  Returns: N/A
//  Side Effect: A new SpriteFont is created from image a_image.
//               The colour (red, green, blue) is used to
//               indicate the width and height of the
//               characters.
//
	SpriteFont (const char* a_image,
	            unsigned char red,
	            unsigned char green,
	            unsigned char blue);

//
//  Constructor
//
//  Purpose: To create a new SpriteFont with the specifed image.
//  Parameter(s):
//    <1> image: The file name of the font image
//    <2> red
//    <3> green
//    <4> blue: The colour indicating the width of the
//              characters in the font
//  Precondition(s):
//    <1> File a_image exists
//    <2> File a_image is a valid 24-bit bitmap file
//    <3> The width of a_image is at least 16
//    <4> The width of a_image is a power of 2
//    <5> The height of a_image is equal to or half of the width
//        of a_image
//    <6> red != green || red != blue
//  Returns: N/A
//  Side Effect: A new SpriteFont is created from image image.
//               The colour (red, green, blue) is used to
//               indicate the width and height of the
//               characters.
//
	SpriteFont (const std::string& image,
	            unsigned char red,
	            unsigned char green,
	            unsigned char blue);

//
//  Destructor
//
//  Purpose: To safely destroy a SpriteFont without memory
//           leaks.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: All dynamically alloacted memory is freed.
//               This include video memory.
//
	~SpriteFont ();

//
//  isInitalized
//
//  Purpose: To determine if this SpriteFont is initalized.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: Whether this SpriteFont is intalized.
//  Side Effect: N/A
//
	bool isInitalized () const;

//
//  is8Bit
//
//  Purpose: To determine if this SpriteFont is an 8-bit font,
//           with 256 characters, instead of a 7-bit font, with
//           128 characters.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isInitalized()
//  Returns: Whether this SpriteFont is a 8-bit font.
//  Side Effect: N/A
//
	bool is8Bit () const;

//
//  getHeight
//
//  Purpose: To determine the height of a character in this
//           SpriteFont.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isInitalized()
//  Returns: The height of a character in this SpriteFont with
//           PLAIN formatting.
//  Side Effect: N/A
//
	int getHeight () const;

//
//  getHeight
//
//  Purpose: To determine the height of a character in this
//           SpriteFont.
//  Parameter(s):
//    <1> format: The text format
//  Precondition(s):
//    <1> isInitalized()
//    <2> isValidFormat(format)
//  Returns: The height of a character in this SpriteFont with
//           formatting format.
//  Side Effect: N/A
//
	int getHeight (unsigned int format) const;

//
//  getWidth
//
//  Purpose: To determine the width of the specifed character in
//           this SpriteFont.
//  Parameter(s):
//    <1> character: The character
//  Precondition(s):
//    <1> isInitalized()
//  Returns: The width of character in this SpriteFont with
//           PLAIN formatting.  If character == '\t', this
//           function returns the same value as getTabWidth.
//  Side Effect: N/A
//
	int getWidth (char character) const;

//
//  getWidth
//
//  Purpose: To determine the width of the specifed character in
//           this SpriteFont if displayed with the specified
//           formatting.
//  Parameter(s):
//    <1> character: The character
//    <2> format: The text format
//  Precondition(s):
//    <1> isInitalized()
//    <2> isValidFormat(format)
//  Returns: The width of character in this SpriteFont with
//           formatting format.  If character == '\t', this
//           function returns the same value as getTabWidth.
//  Side Effect: N/A
//
	int getWidth (char character, unsigned int format) const;

//
//  getTabWidth
//
//  Purpose: To determine the current tab spacing for this
//           SpriteFont.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isInitalized()
//  Returns: The current seperation between tab stops in pixels.
//  Side Effect: N/A
//
	int getTabWidth () const;

//
//  getTabWidthDefault
//
//  Purpose: To determine the default tab spacing for this
//           SpriteFont.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isInitalized()
//  Returns: The default seperation between tab stops in pixels.
//  Side Effect: N/A
//
	int getTabWidthDefault () const;

//
//  getWidth
//
//  Purpose: To determine the width of the specifed string when
//           displayed with this this SpriteFont.
//  Parameter(s):
//    <1> a_str: The NULL-terminated string
//    <1> str: The string
//  Precondition(s):
//    <1> isInitalized()
//    <2> a_str != NULL
//  Returns: The width of a_str/str in this SpriteFont with
//           PLAIN formatting.
//  Side Effect: N/A
//
	int getWidth (const char* a_str) const;
	int getWidth (const std::string& str) const;

//
//  getWidth
//
//  Purpose: To determine the width of the specifed string when
//           displayed with this this SpriteFont with the
//           specified formatting.
//  Parameter(s):
//    <1> a_str: The NULL-terminated string
//    <1> str: The string
//    <2> format: The text format
//  Precondition(s):
//    <1> isInitalized()
//    <2> isValidFormat(format)
//    <3> a_str != NULL
//  Returns: The width of a_str/str in this SpriteFont with
//           formatting format.
//  Side Effect: N/A
//
	int getWidth (const char* a_str,
	              unsigned int format) const;
	int getWidth (const std::string& str,
	              unsigned int format) const;

//
//  getBreakPoint
//
//  Purpose: To determine where a string should be broken to
//           make lines of the specified width when it is
//           displayed in this SpriteFont.
//  Parameter(s):
//    <1> a_str: The NULL-terminated string to break
//    <1> str: The string to break
//    <2> width: The width of a line
//  Precondition(s):
//    <1> isInitalized()
//    <2> a_str != NULL
//  Returns: The character after which a_str/str, if displayed
//           with this SpriteFont and PLAIN formatting, should
//           be broken and continue on the next line.  If
//           a_str/str is short enough to fit on one line,
//           NO_BREAK_NEEDED is returned.
//  Side Effect: N/A
//
	unsigned int getBreakPoint (const char* a_str,
	                            unsigned int width) const;
	unsigned int getBreakPoint (const std::string& str,
	                            unsigned int width) const;

//
//  getBreakPoint
//
//  Purpose: To determine where a string should be broken to
//           make lines of the specified width when it is
//           displayed in this SpriteFont with the specified
//           formatting.
//  Parameter(s):
//    <1> a_str: The NULL-terminated string to break
//    <1> str: The string to break
//    <2> width: The width of a line
//    <3> format: The text format
//  Precondition(s):
//    <1> isInitalized()
//    <2> isValidFormat(format)
//    <3> a_str != NULL
//  Returns: The character after which a_str/str, if displayed
//           with this SpriteFont and formatting format, should
//           be broken and continue on the next line.  If
//           a_str/str is short enough to fit on one line,
//           NO_BREAK_NEEDED is returned.
//  Side Effect: N/A
//
	unsigned int getBreakPoint (const char* a_str,
	                            unsigned int width,
	                            unsigned int format) const;
	unsigned int getBreakPoint (const std::string& str,
	                            unsigned int width,
	                            unsigned int format) const;

//
//  breakString
//
//  Purpose: To break a string into a vector of strings with
//           lines of the specified width when displayed in this
//           SpriteFont.
//  Parameter(s):
//    <1> a_str: The NULL-terminated string to break
//    <1> str: The string to break
//    <2> width: The width of a line
//  Precondition(s):
//    <1> isInitalized()
//    <2> a_str != NULL
//  Returns: A vector containing the lines that a_str/str, if
//           displayed with this SpriteFont and PLAIN
//           formatting, should be broken into.
//  Side Effect: N/A
//
	std::vector<std::string> breakString (
	                                  const char* a_str,
	                                  unsigned int width) const;
	std::vector<std::string> breakString (
	                                  const std::string& str,
	                                  unsigned int width) const;

//
//  breakString
//
//  Purpose: To break a string into a vector of strings with
//           lines of the specified width when displayed in this
//           SpriteFont with the specified formatting.
//  Parameter(s):
//    <1> a_str: The NULL-terminated string to break
//    <1> str: The string to break
//    <2> width: The width of a line
//    <3> format: The text format
//  Precondition(s):
//    <1> isInitalized()
//    <2> isValidFormat(format)
//    <3> a_str != NULL
//  Returns: A vector containing the lines that a_str/str, if
//           displayed with this SpriteFont and formatting
//           format, should be broken into.
//  Side Effect: N/A
//
	std::vector<std::string> breakString (
	                                 const char* a_str,
	                                 unsigned int width,
	                                 unsigned int format) const;
	std::vector<std::string> breakString (
	                                 const std::string& str,
	                                 unsigned int width,
	                                 unsigned int format) const;

//
//  draw
//
//  Purpose: To draw the specified string with the specified
//           formatting in white at the specified position using
//           this SpriteFont.
//  Parameter(s):
//    <1> a_str: The NULL-terminated string to draw
//    <1> str: The string to break
//    <2> x
//    <3> y: The top left corner of the string
//    <4> format: The text format
//  Precondition(s):
//    <1> isInitalized()
//    <2> isValidFormat(format)
//    <3> a_str != NULL
//  Returns: N/A
//  Side Effect: String a_str/str is drawn in white at position
//               (x, y) and with formatting format.  a_str/str
//               is drawn on the XY-plane with -Y as up and each
//               character to the +X of the previous one.
//
	void draw (const char* a_str,
	           double x,
	           double y) const;
	void draw (const std::string& str,
	           double x,
	           double y) const;
	void draw (const char* a_str,
	           double x,
	           double y,
	           unsigned int format) const;
	void draw (const std::string& str,
	           double x,
	           double y,
	           unsigned int format) const;

//
//  draw
//
//  Purpose: To draw the specified string with the specified
//           formatting in the specified colour at the specified
//           position using this SpriteFont.
//  Parameter(s):
//    <1> a_str: The NULL-terminated string to draw
//    <1> str: The string to break
//    <2> x
//    <3> y: The top left corner of the string
//    <4> red
//    <5> green
//    <6> blue: The colour to draw the string with
//    <7> format: The text format
//  Precondition(s):
//    <1> isInitalized()
//    <2> isValidFormat(format)
//    <3> a_str != NULL
//  Returns: N/A
//  Side Effect: String a_str/str is drawn in colour
//               (red, green, blue) and with formatting format
//               at position (x, y).  a_str/str is drawn on the
//               XY-plane with -Y as up and each character to
//               the +X of the previous one.
//
	void draw (const char* a_str,
	           double x,
	           double y,
	           unsigned char red,
	           unsigned char green,
	           unsigned char blue) const;
	void draw (const std::string& str,
	           double x,
	           double y,
	           unsigned char red,
	           unsigned char green,
	           unsigned char blue) const;
	void draw (const char* a_str,
	           double x,
	           double y,
	           unsigned char red,
	           unsigned char green,
	           unsigned char blue,
	           unsigned int format) const;
	void draw (const std::string& str,
	           double x,
	           double y,
	           unsigned char red,
	           unsigned char green,
	           unsigned char blue,
	           unsigned int format) const;

//
//  draw
//
//  Purpose: To draw the specified string with the specified
//           formatting in the specified colour and transparency
//           at the specified position using this SpriteFont.
//  Parameter(s):
//    <1> a_str: The NULL-terminated string to draw
//    <1> str: The string to break
//    <2> x
//    <3> y: The top left corner of the string
//    <4> red
//    <5> green
//    <6> blue: The colour to draw the string with
//    <7> alpha: The transparency to draw the string with
//    <8> format: The text format
//  Precondition(s):
//    <1> isInitalized()
//    <2> isValidFormat(format)
//    <3> a_str != NULL
//  Returns: N/A
//  Side Effect: String a_str/str is drawn in colour
//               (red, green, blue, alpha) and with formatting
//               format at position (x, y).  a_str/str is drawn
//               on the XY-plane with -Y as up and each
//               character to the +X of the previous one.
//
	void draw (const char* a_str,
	           double x,
	           double y,
	           unsigned char red,
	           unsigned char green,
	           unsigned char blue,
	           unsigned char alpha,
	           unsigned int format) const;
	void draw (const std::string& str,
	           double x,
	           double y,
	           unsigned char red,
	           unsigned char green,
	           unsigned char blue,
	           unsigned char alpha,
	           unsigned int format) const;

//
//  draw
//
//  Purpose: To draw the specified string with the specified
//           formatting in the specified colour and transparency
//           at the specified position and depth using this
//           SpriteFont.
//  Parameter(s):
//    <1> a_str: The NULL-terminated string to draw
//    <1> str: The string to break
//    <2> x
//    <3> y: The top left corner of the string
//    <4> depth: The depth (z) to display the text at
//    <5> red
//    <6> green
//    <7> blue: The colour to draw the string with
//    <8> alpha: The transparency to draw the string with
//    <9> format: The text format
//  Precondition(s):
//    <1> isInitalized()
//    <2> depth >= 0.0
//    <3> depth <= 1.0
//    <4> isValidFormat(format)
//    <5> a_str != NULL
//  Returns: N/A
//  Side Effect: String a_str/str is drawn in colour
//               (red, green, blue, alpha) and with formatting
//               format at position (x, y)and depth depth.
//               a_str/str is drawn on the XY-plane with -Y as
//               up and each character to the +X of the previous
//               one.
//
	void draw (const char* a_str,
	           double x,
	           double y,
	           double depth,
	           unsigned char red,
	           unsigned char green,
	           unsigned char blue,
	           unsigned char alpha,
	           unsigned int format) const;
	void draw (const std::string& str,
	           double x,
	           double y,
	           double depth,
	           unsigned char red,
	           unsigned char green,
	           unsigned char blue,
	           unsigned char alpha,
	           unsigned int format) const;

//
//  draw
//
//  Purpose: To draw the specified vector of strings with the
//           specified formatting in white at the specified
//           position using this SpriteFont.
//  Parameter(s):
//    <1> lines: The vector of strings
//    <2> x
//    <3> y: The top left corner of the string
//    <4> format: The text format
//  Precondition(s):
//    <1> isInitalized()
//    <2> isValidFormat(format)
//  Returns: N/A
//  Side Effect: The strings in lines are drawn in white as
//               successive lines of text starting at position
//               (x, y) and with formatting format.  The strings
//               is drawn on the XY-plane with -Y as up and each
//               character to the +X of the previous one and
//               each line to the +Y of the previous one.
//
	void draw (const std::vector<std::string>& lines,
	           double x,
	           double y) const;
	void draw (const std::vector<std::string>& lines,
	           double x,
	           double y,
	           unsigned int format) const;

//
//  draw
//
//  Purpose: To draw the specified vector of strings with the
//           specified formatting in the specified colour at the
//           specified position using this SpriteFont.
//  Parameter(s):
//    <1> lines: The vector of strings
//    <2> x
//    <3> y: The top left corner of the string
//    <4> red
//    <5> green
//    <6> blue: The colour to draw the string with
//    <7> format: The text format
//    <8> alpha: The transparency to draw the string with
//  Precondition(s):
//    <1> isInitalized()
//    <2> isValidFormat(format)
//  Returns: N/A
//  Side Effect: The strings in lines are drawn in colour
//               (red, green, blue) as successive lines of text
//               starting at position (x, y) and with formatting
//               format.  The strings is drawn on the XY-plane
//               with -Y as up and each character to the +X of
//               the previous one and each line to the +Y of the
//               previous one.
//
	void draw (const std::vector<std::string>& lines,
	           double x,
	           double y,
	           unsigned char red,
	           unsigned char green,
	           unsigned char blue) const;
	void draw (const std::vector<std::string>& lines,
	           double x,
	           double y,
	           unsigned char red,
	           unsigned char green,
	           unsigned char blue,
	           unsigned int format) const;

//
//  draw
//
//  Purpose: To draw the specified vector of strings with the
//           specified formatting in the specified colour and
//           transparency at the specified position using this
//           SpriteFont.
//  Parameter(s):
//    <1> lines: The vector of strings
//    <2> x
//    <3> y: The top left corner of the string
//    <4> red
//    <5> green
//    <6> blue: The colour to draw the string with
//    <7> alpha: The transparency to draw the string with
//    <8> format: The text format
//  Precondition(s):
//    <1> isInitalized()
//    <2> isValidFormat(format)
//  Returns: N/A
//  Side Effect: The strings in lines are drawn in colour
//               (red, green, blue, alpha) as successive lines
//               of text with formatting format, starting at
//               position (x, y).  The strings are drawn on the
//               XY-plane with -Y as up and each character to
//               the +X of the previous one and each line to the
//               +Y of the previous one.
//
	void draw (const std::vector<std::string>& lines,
	           double x,
	           double y,
	           unsigned char red,
	           unsigned char green,
	           unsigned char blue,
	           unsigned char alpha,
	           unsigned int format) const;

//
//  draw
//
//  Purpose: To draw the specified vector of strings with the
//           specified formatting in the specified colour and
//           transparency at the specified position using this
//           SpriteFont.
//  Parameter(s):
//    <1> lines: The vector of strings
//    <2> x
//    <3> y: The top left corner of the string
//    <4> depth: The depth (z) to display the text at
//    <5> red
//    <6> green
//    <7> blue: The colour to draw the string with
//    <8> alpha: The transparency to draw the string with
//    <9> format: The text format
//  Precondition(s):
//    <1> isInitalized()
//    <2> depth >= 0.0
//    <3> depth <= 1.0
//    <4> isValidFormat(format)
//  Returns: N/A
//  Side Effect: The strings in lines are drawn in colour
//               (red, green, blue, alpha) as successive lines
//               of text with formatting format, starting at
//               position (x, y) and at depth depth.  The
//               strings are drawn on the XY-plane with -Y as up
//               and each character to the +X of the previous
//               one and each line to the +Y of the previous
//               one.
//
	void draw (const std::vector<std::string>& lines,
	           double x,
	           double y,
	           double depth,
	           unsigned char red,
	           unsigned char green,
	           unsigned char blue,
	           unsigned char alpha,
	           unsigned int format) const;

//
//  load
//
//  Purpose: To initalize this SpriteFont with the font in the
//           specified file.
//  Parameter(s):
//    <1> a_image: The file name of the font image
//    <1> image: The file name of the font image
//  Precondition(s):
//    <1> !isInitalized()
//    <2> a_image != NULL
//    <3> File a_image exists
//    <4> File a_image is a valid 24-bit of 32-bit bitmap file
//    <5> The width of image/a_image is at least 16
//    <6> The width of image/a_image is a power of 2
//    <7> The height of image/a_image is equal to exactly or
//        half of the width of image/a_image
//  Returns: N/A
//  Side Effect: This SpriteFont is initialized with image
//               a_image/image. The colour magenta is used to
//               indicate the size of the characters.
//
	void load (const char* a_image);
	void load (const std::string& image);

//
//  load
//
//  Purpose: To initalize this SpriteFont with the font in the
//           specified file.
//  Parameter(s):
//    <1> a_image: The file name of the font image
//    <1> image: The file name of the font image
//    <2> red
//    <3> green
//    <4> blue: The colour indicating the size of the characters
//              in the font
//  Precondition(s):
//    <1> !isInitalized()
//    <2> a_image != NULL
//    <3> File a_image exists
//    <4> File a_image is a valid 24-bit of 32-bit bitmap file
//    <5> The width of image/a_image is at least 16
//    <6> The width of image/a_image is a power of 2
//    <7> The height of image/a_image is equal to exactly or
//        half of the width of image/a_image
//    <8> red != green || red != blue
//  Returns: N/A
//  Side Effect: This SpriteFont is initialized with image
//               a_image/image. The colour (red, green, blue) is
//               used to indicate the size of the characters.
//
	void load (const char* a_image,
	           unsigned char red,
	           unsigned char green,
	           unsigned char blue);
	void load (const std::string& image,
	           unsigned char red,
	           unsigned char green,
	           unsigned char blue);

//
//  setTabWidthPixels
//
//  Purpose: To change the spacing of the tab stops for this
//           SpriteFont to the specified size in pixels.
//  Parameter(s):
//    <1> width: The new tab width in pixels
//  Precondition(s):
//    <1> isInitalized()
//    <2> width > 0
//  Returns: The tab stops for this SpriteFont are set to be
//           spaced width pixels apart.  Individual tabs may be
//           smaller than this.
//  Side Effect: N/A
//
	void setTabWidthPixels (int width);

//
//  setTabWidthMultiple
//
//  Purpose: To change the spacing of the tab stops for this
//           SpriteFont to the width of the specified character,
//           repreated the specified number of times.
//  Parameter(s):
//    <1> character: The character to use as the basis for the
//                   tab width
//    <2> factor: The number of times the width of the character
//                that the tab should be
//  Precondition(s):
//    <1> isInitalized()
//    <2> character != '\n'
//    <3> character != '\t'
//    <4> factor > 0.0
//  Returns: The tab stops for this SpriteFont are set to be
//           spaced factor times the width of character
//           character apart.  The tab spacing will always be at
//           least one pixel.  Individual tabs may be smaller
//           than the new tab spacing.
//  Side Effect: N/A
//
	void setTabWidthMultiple (char character, double factor);

//
//  setTabWidthToDefault
//
//  Purpose: To restore the deafult spacing of tab stops for
//           this SpriteFont.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isInitalized()
//  Returns: The tab stops for this SpriteFont are set to have
//           the standard size.  Individual tabs may be smaller.
//  Side Effect: N/A
//
	void setTabWidthToDefault ();

private:
//
//  Copy Constructor
//  Assignment Operator
//
//  These functions have intentionally not been implemented
//    because video memory should not be copied.
//
	SpriteFont (const SpriteFont& original);
	SpriteFont& operator= (const SpriteFont& original);

//
//  Helper Function: setUpForDrawing
//
//  Purpose: To set up the drawing state for drawing text.
//  Parameter(s):
//    <1> depth: The depth (z) to draw the text at
//    <2> red
//    <3> green
//    <4> blue: The colour to draw the string with
//    <5> alpha: The transparency to draw the string with
//    <6> format: The text format
//  Precondition(s):
//    <1> isInitalized()
//    <2> depth >= 0.0
//    <3> depth <= 1.0
//    <4> isValidFormat(format)
//  Returns: N/A
//  Side Effect: The OpenGL drawing state is set to what is
//               needed for drawing text at depth depth using
//               this SpriteFont.
//
	void setUpForDrawing (double depth,
	                      unsigned char red,
	                      unsigned char green,
	                      unsigned char blue,
	                      unsigned char alpha,
	                      unsigned int format) const;

//
//  Helper Function: drawLineOfText
//
//  Purpose: To draw a line of text after the drawing state has
//           been set up.
//  Parameter(s):
//    <1> str: The string to break
//    <2> x
//    <3> y: The top left corner of the string
//    <4> depth: The depth (z) to draw the text at
//    <5> format: The text format
//  Precondition(s):
//    <1> isInitalized()
//    <2> depth >= 0.0
//    <3> depth <= 1.0
//    <4> isValidFormat(format)
//  Returns: The Y coordinate when the end of the string is
//           displayed.  This will be larger than y if str
//           contains newline characters.
//  Side Effect: The text in stirn str is displayed with format
//               format at position (x, y) at depth depth.
//
	double drawLineOfText (const std::string& str,
	                       double x,
	                       double y,
	                       double depth,
	                       unsigned int format) const;

//
//  Helper Function: unsetUpForDrawing
//
//  Purpose: To restore the drawing state after drawing text.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isInitalized()
//  Returns: N/A
//  Side Effect: Whatever needs to be done after drawing text is
//               done.
//
	void unsetUpForDrawing () const;

//
//  Helper Function: invariant
//
//  Purpose: To determine if the class invariant is true.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: Whether the class invariant is true.
//  Side Effect: N/A
//
	bool invariant () const;

private:
	unsigned int m_character_count;
	unsigned int m_image_size;
	unsigned int m_character_height;

#ifdef OBJ_LIBRARY_SHADER_DISPLAY
	// one 2d texture array with a layer for each character
	unsigned int m_texture_name;
#else
	// array of textures, one per character
	unsigned int ma_character_name[CHARACTER_COUNT_MAX];
#endif

	unsigned int ma_character_width[CHARACTER_COUNT_MAX];
};



}  // end of namespace ObjLibrary

#endif
