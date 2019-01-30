//
//  TextureManager.h
//
//  A global service to handle Textures.
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

#ifndef OBJ_LIBRARY_TEXTURE_MANAGER_H
#define OBJ_LIBRARY_TEXTURE_MANAGER_H

#include <string>
#include <iostream>



namespace ObjLibrary
{

class Vector3;
class Texture;



//
//  TextureManager
//
//  A global service to handle Textures.  This module keeps
//    track of which textures have already been loaded.  This
//    allows the same teture to be used in multiple parts of the
//    program while only being in memory once.  This can occur,
//    for example, if more than one model uses the same texture
//    file.
//
//  Normally, textures are loaded and used with two functions:
//    <1> get returns a reference to the texture
//    <2> activate sets the current openGL 2D texture to the
//        texture specified
//
//  Both of these functions will load the texture with default
//    parameters if it has not already been loaded.  If you need
//    additional loading parameters, first call the load
//    function for the texture.  Parameters available with the
//    load function include:
//    -> a logging file/stream to write errors to
//    -> wrapping and min/magnification options
//    -> a transparent colour
//
//  Name comparisons are always case-insensitive.
//
namespace TextureManager
{

//
//  TEXTURE_INDEX_INVALID
//
//  A constant indicating that a texture dioes not have an
//    index.  This occurs in 2 situations:
//    -> When an error is encountered loading a texture
//    -> When querying the index of a non-existant texture
//
const unsigned int TEXTURE_INDEX_INVALID = ~0u;



//
//  getCount
//
//  Purpose: To determine the number of textures loaded.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: The number of textures loaded.
//  Side Effect: N/A
//
unsigned int getCount ();

//
//  getName
//
//  Purpose: To determine the name of the texture with the
//           specified index.
//  Parameter(s):
//    <1> index: Which texture
//  Precondition(s):
//    <1> index < getCount()
//  Returns: The name of the texture with index index.
//  Side Effect: N/A
//
const std::string& getName (unsigned int index);

//
//  get
//
//  Purpose: To retrieve a reference to the texture with the
//           specified index.
//  Parameter(s):
//    <1> index: Which texture
//  Precondition(s):
//    <1> index < getCount()
//  Returns: A reference to the texture with index index.
//  Side Effect: N/A
//
const Texture& get (unsigned int index);

//
//  get
//
//  Purpose: To retrieve a reference to the texture with the
//           specified name.
//  Parameter(s):
//    <1> a_name: The name of the texture
//  Precondition(s):
//    <1> a_name != NULL
//  Returns: The Texture with name a_name.  If there is no
//           texture loaded with name a_name, and a_name ends in
//           a case-insensitive ".bmp" (or ".png", depending on
//           settings), a new Texture loaded from the file named
//           a_name is returned.  Otherwise an all-white dummy
//           texture is returned.
//  Side Effect: If the texture has not been loaded, and a
//               suitable file exists, the texture is loaded
//               and error messages may be generated.  If the
//               dummy texture is first loaded, an error message
//               is always printed.  If the texture has already
//               been loaded, no error messages will be printed.
//
const Texture& get (const char* a_name);

//
//  get
//
//  Purpose: To retrieve a reference to the texture with the
//           specified name.
//  Parameter(s):
//    <1> name: The name of the texture
//  Precondition(s): N/A
//  Returns: The Texture with name name.  If there is no texture
//           loaded with name name, and name ends in a
//           case-insensitive ".bmp" (or ".png", depending on
//           settings), a new Texture loaded from the file named
//           a_name is returned.  Otherwise an all-white dummy
//           texture is returned.
//  Side Effect: If the texture has not been loaded, and a
//               suitable file exists, the texture is loaded
//               and error messages may be generated.  If the
//               dummy texture is first loaded, an error message
//               is always printed.  If the texture has already
//               been loaded, no error messages will be printed.
//
const Texture& get (const std::string& name);

//
//  activate
//
//  Purpose: To activate the texture with the specified index.
//  Parameter(s):
//    <1> index: Which texture
//  Precondition(s):
//    <1> index < getCount()
//  Returns: The texture with index index is activated.
//  Side Effect: N/A
//
void activate (unsigned int index);

//
//  activate
//
//  Purpose: To activate the texture with the specified name.
//  Parameter(s):
//    <1> a_name: The name of the texture
//  Precondition(s):
//    <1> a_name != NULL
//  Returns: N/A
//  Side Effect: The Texture with name a_name is activated.  If
//               there is no texture loaded with name a_name,
//               and a_name ends in a case-insensitive ".bmp"
//               (or ".png", depending on settings), a new
//               Texture loaded from the file named a_name is
//               returned.  Otherwise an all-white dummy texture
//               is activated.  This function may generate error
//               messages.  If the dummy texture is first
//               loaded, an error message is always printed.  If
//               the texture has already been loaded, no error
//               messages will be printed.
//
void activate (const char* a_name);

//
//  activate
//
//  Purpose: To activate the texture with the specified name.
//  Parameter(s):
//    <1> name: The name of the texture
//  Precondition(s):
//    <1> logfile != ""
//  Returns: N/A
//  Side Effect: The Texture with name name is activated.  If
//               there is no texture loaded with name name, and
//               name ends in a case-insensitive ".bmp" (or
//               ".png", depending on settings), a new Texture
//               loaded from the file named a_name is returned.
//               Otherwise an all-white dummy texture is
//               activated.  This function may generate error
//               messages.  If the dummy texture is first
//               loaded, an error message is always printed.  If
//               the texture has already been loaded, no error
//               messages will be printed.
//
void activate (const std::string& name);

//
//  isLoaded
//
//  Purpose: To determine if a texture with the specified name
//           exists.
//  Parameter(s):
//    <1> a_name: The name of the texture
//  Precondition(s):
//    <1> a_name != NULL
//  Returns: Whether there is a texture with name a_name.
//  Side Effect: N/A
//
bool isLoaded (const char* a_name);

//
//  isLoaded
//
//  Purpose: To determine if a texture with the specified name
//           exists.
//  Parameter(s):
//    <1> name: The name of the texture
//  Precondition(s): N/A
//  Returns: Whether there is a texture with name name.
//  Side Effect: N/A
//
bool isLoaded (const std::string& name);

//
//  getIndex
//
//  Purpose: To determine the index of the texture with the
//           specified name.
//  Parameter(s):
//    <1> a_name: The name of the texture
//  Precondition(s):
//    <1> a_name != NULL
//  Returns: The index of texture a_name.  If there is no
//           texture by that name loaded, TEXTURE_INDEX_INVALID
//           is returned.
//  Side Effect: N/A
//
unsigned int getIndex (const char* a_name);

//
//  getIndex
//
//  Purpose: To determine the index of the texture with the
//           specified name.
//  Parameter(s):
//    <1> name: The name of the texture
//  Precondition(s): N/A
//  Returns: The index of texture name.  If there is no texture
//           by that name loaded, TEXTURE_INDEX_INVALID is
//           returned.
//  Side Effect: N/A
//
unsigned int getIndex (const std::string& name);

//
//  isDummyTexture
//
//  Purpose: To determine if the specified Texture is the dummy
//           texture returned when a get command fails.
//  Parameter(s):
//    <1> texture: The texture
//  Precondition(s): N/A
//  Returns: Whether texture is the dummy Texture returned by
//           the get function.
//  Side Effect: N/A
//
bool isDummyTexture (const Texture& texture);

//
//  add
//
//  Purpose: To add the texture with the specified OpenGL
//           texture name to the texture manager.
//  Parameter(s):
//    <1> opengl_name: The OpenGL texture name for the texture
//    <2> a_name: The name of the texture to add
//  Precondition(s):
//    <1> opengl_name != 0
//    <2> a_name != NULL
//    <3> !isLoaded(a_name)
//  Returns: The index that the texturee with name opengl_name
//           was added at.
//  Side Effect: The texture with OpenGL texture name
//               opengl_name is added to the texture manager
//               under the name a_name.
//
unsigned int add (unsigned int opengl_name,
                  const char* a_name);

//
//  add
//
//  Purpose: To add the texture with the specified OpenGL
//           texture name to the texture manager.
//  Parameter(s):
//    <1> opengl_name: The OpenGL texture name for the texture
//    <2> name: The name of the texture
//  Precondition(s):
//    <1> opengl_name != 0
//    <2> !isLoaded(name)
//  Returns: The index that the texturee with name opengl_name
//           was added at.
//  Side Effect: The texture with OpenGL texture name
//               opengl_name is added to the texture manager
//               under the name name.
//
unsigned int add (unsigned int opengl_name,
                  const std::string& name);

//
//  add
//
//  Purpose: To add the specified Texture to the texture manager
//           with the specified name.
//  Parameter(s):
//    <1> texture: The Texture
//    <2> a_name: The name of the texture to add
//  Precondition(s):
//    <1> texture.isSet()
//    <2> a_name != NULL
//    <3> !isLoaded(a_name)
//  Returns: The index that texture texture was added at.
//  Side Effect: Texture texture is added to the texture manager
//               under the name a_name.
//
unsigned int add (const Texture& texture,
                  const char* a_name);

//
//  add
//
//  Purpose: To add the specified Texture to the texture manager
//           with the specified name.
//  Parameter(s):
//    <1> texture: The Texture
//    <2> name: The name of the texture
//  Precondition(s):
//    <1> texture.isSet()
//    <2> !isLoaded(name)
//  Returns: The index that texture texture was added at.
//  Side Effect: Texture texture is added to the texture manager
//               under the name name.
//
unsigned int add (const Texture& texture,
                  const std::string& name);

//
//  load
//
//  Purpose: To load a texture with the specified name.
//  Parameter(s):
//    <1> a_name: The name of the texture
//    <1> name: The name of the texture
//    <2> logfile: The file to write loading errors to
//    <2> r_logstream: The stream to write loading errors to
//  Precondition(s):
//    <1> a_name != NULL
//    <2> !isLoaded(a_name)
//    <2> !isLoaded(name)
//    <3> logfile != ""
//  Returns: The index of the texture loaded.  If the texture
//           could not be loaded, TEXTURE_LOADING_ERROR is
//           returned.
//  Side Effect: If a suitable file with name a_name/name
//               exists, the texture is loaded and error
//               messages may be generated.  If a logfile or
//               logging stream is specified, any loading errors
//               are written to that file or stream.  Otherwise,
//               any loading errors are written to the standard
//               error stream.  If the file does not exist or is
//               unsuitable, the texture is not loaded and an
//               error message is printed.
//
unsigned int load (const char* a_name);
unsigned int load (const char* a_name,
                   const std::string& logfile);
unsigned int load (const char* a_name,
                   std::ostream& r_logstream);
unsigned int load (const std::string& name);
unsigned int load (const std::string& name,
                   const std::string& logfile);
unsigned int load (const std::string& name,
                   std::ostream& r_logstream);

//
//  load
//
//  Purpose: To load a texture with the specified name, wrapping
//           mode, and magification and minification filters.
//  Parameter(s):
//    <1> a_name: The name of the texture
//    <1> name: The name of the texture
//    <2> wrap_s:
//    <3> wrap_t: The behaviour of the texture outside of the
//                range [0, 1) along the x-/y-axis
//    <4> mag_filter: The magnification filter
//    <5> min_filter: The minification filter
//    <6> logfile: The file to write loading errors to
//    <6> r_logstream: The stream to write loading errors to
//  Precondition(s):
//    <1> a_name != NULL
//    <2> !isLoaded(a_name)
//    <2> !isLoaded(name)
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
//    <3> wrap_s == GL_REPEAT || wrap_s == GL_CLAMP
//    <4> wrap_t == GL_REPEAT || wrap_t == GL_CLAMP
#else
//    <3> wrap_s == GL_REPEAT ||
//        wrap_s == GL_MIRRORED_REPEAT ||
//        wrap_s == GL_CLAMP_TO_EDGE ||
//        wrap_s == GL_CLAMP_TO_BORDER
//    <4> wrap_t == GL_REPEAT ||
//        wrap_t == GL_MIRRORED_REPEAT ||
//        wrap_t == GL_CLAMP_TO_EDGE ||
//        wrap_t == GL_CLAMP_TO_BORDER
#endif
//    <4> mag_filter == GL_NEAREST ||
//        mag_filter == GL_LINEAR
//    <5> min_filter == GL_NEAREST ||
//        min_filter == GL_LINEAR ||
//        min_filter == GL_NEAREST_MIPMAP_NEAREST ||
//        min_filter == GL_NEAREST_MIPMAP_LINEAR ||
//        min_filter == GL_LINEAR_MIPMAP_NEAREST ||
//        min_filter == GL_LINEAR_MIPMAP_LINEAR
//    <6> logfile != ""
//  Returns: The index of the texture loaded.  If the texture
//           could not be loaded, TEXTURE_LOADING_ERROR is
//           returned.
//  Side Effect: If a suitable file with name a_name/name
//               exists, the texture is loaded and error
//               messages may be generated.  The texture loaded
//               has wrapping modes wrap_s and wrap_t in the S
//               and T directions, magnification filter
//               mag_filter, and minification filter min_filter.
//               If a logfile or logging stream is specified,
//               any loading errors are written to that file or
//               stream.  Otherwise, any loading errors are
//               written to the standard error stream.  If the
//               file does not exist or is unsuitable, the
//               texture is not loaded and an error message is
//               printed.
//
unsigned int load (const char* a_name,
                   unsigned int wrap_s,
                   unsigned int wrap_t,
                   unsigned int mag_filter,
                   unsigned int min_filter);
unsigned int load (const char* a_name,
                   unsigned int wrap_s,
                   unsigned int wrap_t,
                   unsigned int mag_filter,
                   unsigned int min_filter,
                   const std::string& logfile);
unsigned int load (const char* a_name,
                   unsigned int wrap_s,
                   unsigned int wrap_t,
                   unsigned int mag_filter,
                   unsigned int min_filter,
                   std::ostream& r_logstream);
unsigned int load (const std::string& name,
                   unsigned int wrap_s,
                   unsigned int wrap_t,
                   unsigned int mag_filter,
                   unsigned int min_filter);
unsigned int load (const std::string& name,
                   unsigned int wrap_s,
                   unsigned int wrap_t,
                   unsigned int mag_filter,
                   unsigned int min_filter,
                   const std::string& logfile);
unsigned int load (const std::string& name,
                   unsigned int wrap_s,
                   unsigned int wrap_t,
                   unsigned int mag_filter,
                   unsigned int min_filter,
                   std::ostream& r_logstream);

//
//  load
//
//  Purpose: To load a texture with the specified name, wrapping
//           mode, magification and minification filters, and
//           transparent colour.
//  Parameter(s):
//    <1> a_name: The name of the texture
//    <1> name: The name of the texture
//    <2> wrap_s:
//    <3> wrap_t: The behaviour of the texture outside of the
//                range [0, 1) along the x-/y-axis
//    <4> mag_filter: The magnification filter
//    <5> min_filter: The minification filter
//    <6> transparent_colour: The transparency colour
//    <7> logfile: The file to write loading errors to
//    <8> r_logstream: The stream to write loading errors to
//  Precondition(s):
//    <1> a_name != NULL
//    <2> !isLoaded(a_name)
//    <2> !isLoaded(name)
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
//    <3> wrap_s == GL_REPEAT || wrap_s == GL_CLAMP
//    <4> wrap_t == GL_REPEAT || wrap_t == GL_CLAMP
#else
//    <3> wrap_s == GL_REPEAT ||
//        wrap_s == GL_MIRRORED_REPEAT ||
//        wrap_s == GL_CLAMP_TO_EDGE ||
//        wrap_s == GL_CLAMP_TO_BORDER
//    <4> wrap_t == GL_REPEAT ||
//        wrap_t == GL_MIRRORED_REPEAT ||
//        wrap_t == GL_CLAMP_TO_EDGE ||
//        wrap_t == GL_CLAMP_TO_BORDER
#endif
//    <4> mag_filter == GL_NEAREST ||
//        mag_filter == GL_LINEAR
//    <5> min_filter == GL_NEAREST ||
//        min_filter == GL_LINEAR ||
//        min_filter == GL_NEAREST_MIPMAP_NEAREST ||
//        min_filter == GL_NEAREST_MIPMAP_LINEAR ||
//        min_filter == GL_LINEAR_MIPMAP_NEAREST ||
//        min_filter == GL_LINEAR_MIPMAP_LINEAR
//    <6> transparent_colour.isAllComponentsNonNegative()
//    <7> transparent_colour.isAllComponentsLessThanOrEqual(1.0)
//    <8> logfile != ""
//  Returns: The index of the texture loaded.  If the texture
//           could not be loaded, TEXTURE_LOADING_ERROR is
//           returned.
//  Side Effect: If a suitable file with name a_name/name
//               exists, the texture is loaded and error
//               messages may be generated.  The texture loaded
//               has wrapping  modes wrap_s and wrap_t in the S
//               and T directions, magnification filter
//               mag_filter, and minification filter min_filter.
//               All pixels of colour transparent_colour are set
//               to have an alpha value of 0.0,  and all other
//               pixels are set to have an alpha value of 1.0.
//               The existing alpha values in the texture, if
//               any, are ignored.  If a logfile or logging
//               stream is specified, any loading errors are
//               written to that file or stream.  Otherwise, any
//               loading errors are written to the standard
//               error stream.  If the file does not exist or is
//               unsuitable, the texture is not loaded and an
//               error message is printed.
//
unsigned int load (const char* a_name,
                   unsigned int wrap_s,
                   unsigned int wrap_t,
                   unsigned int mag_filter,
                   unsigned int min_filter,
                   const Vector3& transparent_colour);
unsigned int load (const char* a_name,
                   unsigned int wrap_s,
                   unsigned int wrap_t,
                   unsigned int mag_filter,
                   unsigned int min_filter,
                   const Vector3& transparent_colour,
                   const std::string& logfile);
unsigned int load (const char* a_name,
                   unsigned int wrap_s,
                   unsigned int wrap_t,
                   unsigned int mag_filter,
                   unsigned int min_filter,
                   const Vector3& transparent_colour,
                   std::ostream& r_logstream);
unsigned int load (const std::string& name,
                   unsigned int wrap_s,
                   unsigned int wrap_t,
                   unsigned int mag_filter,
                   unsigned int min_filter,
                   const Vector3& transparent_colour);
unsigned int load (const std::string& name,
                   unsigned int wrap_s,
                   unsigned int wrap_t,
                   unsigned int mag_filter,
                   unsigned int min_filter,
                   const Vector3& transparent_colour,
                   const std::string& logfile);
unsigned int load (const std::string& name,
                   unsigned int wrap_s,
                   unsigned int wrap_t,
                   unsigned int mag_filter,
                   unsigned int min_filter,
                   const Vector3& transparent_colour,
                   std::ostream& r_logstream);

//
//  unloadAll
//
//  Purpose: To remove all textures from the texture manager.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: tetxure.
//  Side Effect: All textures are removed from the texture
//               manager.
//
void unloadAll ();

}  // end of namespace TextureManager



}  // end of namespace ObjLibrary

#endif
