//
//  ObjSettings.h
//
//  A module to encapsulate the general settings for the OBJ
//    library.
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

#ifndef OBJ_LIBRARY_OBJ_SETTINGS_H
#define OBJ_LIBRARY_OBJ_SETTINGS_H



//
//  An OBJ file often references one or more MTL material files
//    which in term reference one or more textures.  It is
//    possible to for the path from the OBJ file to be
//    implicitly prepended to the other files.  This is called
//    called path propagation and can be enabled or disabled as
//    through the use of a macro.
//
//  For example, consider the file "barrel.obj" in the "models"
//    subfolder.  The OBJ file references a material library
//    named "barrel.mtl" which in turn references an image named
//    "barrel.bmp".  The barrel is loaded with the filename
//    "models/barrel.obj".  If path propagation is disabled, the
//    material library and image will be loaded with the
//    filenames "barrel.mtl" and "barrel.bmp".  If, in contrast,
//    path propagation is enabled, the filenames used are
//    "models/barrel.mtl" and "models/barrel.bmp".
//
//  WARNING: Enabling and disabling path propagation after
//           models have been loaded can lead to unexpected
//           behaviour.
//
//  To enable path propogation, define the macro
//    OBJ_LIBRARY_PATH_PROPAGATION.
//
#define OBJ_LIBRARY_PATH_PROPAGATION



//
//  By default, the ObjLibrary uses GL_NEAREST for the
//    mag/minification filters for textures.  This options
//    switches between pixels and mipmapping levels at
//    threshold values, which results in the fastest display.
//    However, it can look blocky (if magnified) or shimmery
//    (if minified), and produce visible shifts at the
//    mipmapping distance thresholds.
//
//  To change the texture default to use smooth interpolation
//    between pixels and mipmaps, define the macro
//    OBJ_LIBRARY_LINEAR_TEXTURE_INTERPOLATION.
//
#define OBJ_LIBRARY_LINEAR_TEXTURE_INTERPOLATION



//
//  There is an inconsistancy in *.mtl files.  Some files use a
//    "d" (dissolve) for transparency, which works reliably.
//    Others use a "Tr" line, which does not.  The problem is
//    that some files use 0.0 to represent fully transparent
//    (i.e. invisible) and 1.0 to represent fully opaque.  Other
//    files use them the other way around.
//
//  By default, the ObjLibrary treats "Tr 0.0" as fully
//    transparent.  To treat "Tr 0.0 as fully opaque, define the
//    macro OBJ_LIBRARY_TR_0_IS_OPAQUE.
//
#define OBJ_LIBRARY_TR_0_IS_OPAQUE



//
//  By default, the ObjLibrary only loads textures of type
//    ".bmp".  However, it can also load textures of type
//    ".png".  To do this, you will need the following
//    libraries:
//      <1> ZLib 1.1.3 by Jean-loup Gailly and Mark Adlerun
//      <2> LibPNG 1.0.2 by Guy Eric Schalnat, Group 42, Inc.,
//          Andreas Dilger, Glenn Randers-Pehrson
//      <3> (unnamed) PNG loader library by Ben Wyatt
//
//  The assumed organization of files is as follows:
//    glpng.c
//    glpng.h
//    ObjLibrary
//      ObjLibrary library files
//    png
//      png library files
//    zlib
//      zlib library files
//
//  It would also be possible to use the other libraries as .lib
//    files, but those often lead to portability issues.
//
//  To enable loading of .png files, define the macro
//    OBJ_LIBRARY_LOAD_PNG_TEXTURES.
//
//#define OBJ_LIBRARY_LOAD_PNG_TEXTURES



//
//  The Vector* classes in the ObjLibrary can interface with the
//    OpenGL Mathematics (glm) library.  The glm library
//    provides a lot of the mathematical functions that you need
//    to replace fixed pipeline functionality when converting a
//    program to use shaders.  The glm (header-only) library is
//    available at:
//      http://glm.g-truc.net/0.9.8/index.html
//
//  The assumed organization of files is as follows:
//    ObjLibrary
//      ObjLibrary library files
//    glm
//      glm library files (all are header files)
//
//  To allow the ObjLibrary Vector* classes to convert back and
//    forth with the glm::vec* classes, define the macro:
//    OBJ_LIBRARY_GLM_INTERACTION
//  This macro can also be defined seperately in the Vector.h
//    and Vector3.h files so that they can be used without the
//    full ObjLibrary.
//
//#define OBJ_LIBRARY_GLM_INTERACTION



//
//  By default, models are drawn in fixed-pipeline mode.  There
//    is a more powerful and faster implementation available if
//    your program is running shaders.  For this, you will need:
//      <1> Shader-capable OpenGL using gl3w
//      <2> glm (see above)
//      <3> ObjLibrary shader files
//
//  The assumed organization of files is as follows:
//    gl3w.c
//    gl3w.h
//    glcorearb.c
//    GetGlutWithShaders.=h
//    ObjLibrary
//      ObjLibrary library files
//      ObjLibrary shader files
//
//  Note that shaders are incompatible with fixed-pipeline
//    drawing -- they cannot be combined.  If you enable
//    shaders, many of the standard OpenGL commands will
//    disppear, as will many ObjLibray commands which depend on
//    them.
//
//  To enable shader-based display, define the macro
//    OBJ_LIBRARY_SHADER_DISPLAY.
//
//#define OBJ_LIBRARY_SHADER_DISPLAY

#ifdef OBJ_LIBRARY_SHADER_DISPLAY
	#ifndef OBJ_LIBRARY_GLM_INTERACTION
		#error "OBJ_LIBRARY_GLM_INTERACTION requires OBJ_LIBRARY_GLM_INTERACTION"
	#endif
#endif

//
//  By default, shaders are loaded in whatever version is
//    specified in the shader file.  If desired, the ObjLibrary
//    can preprend the same version number prepended onto each
//    shader.  Shader versions are specified as a major and
//    minor version number.  For example, shader version 4.2 is
//    of major version 4 and minor version 2.
//
//  A version directive in a shader must be the first line.  For
//    shader version 4.2, it would be:
//
//    #version 420
//
//  If you do not define these values and do not specify the
//    shader version in the file, the shader will default to
//    version 1.1.  This is a very bad thing.
//
//  Note that the shader version may be different than the
//    OpenGL version.
//
//  To prepend a specific shader version, define both of the
//    macros OBJ_LIBRARY_SHADER_VERSION_MAJOR and
//    OBJ_LIBRARY_SHADER_VERSION_MINOR.
//
//#define OBJ_LIBRARY_SHADER_VERSION_MAJOR 4
//#define OBJ_LIBRARY_SHADER_VERSION_MINOR 4

#if defined (OBJ_LIBRARY_SHADER_VERSION_MAJOR) && \
    defined (OBJ_LIBRARY_SHADER_VERSION_MINOR)
	#define OBJ_LIBRARY_PREPEND_SHADER_VERSION
#endif




#endif
