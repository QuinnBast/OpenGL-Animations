//
//  GetGlut.h
//
//  A header file in import OpenGL, GLU, and GLUT on Windows,
//    Mac OSX, Linux, or Solaris.  The intermediate file is
//    helpful because differant includesd are needed on
//    differant platforms.
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

#ifndef OBJ_LIBRARY_GET_GLUT_H
#define OBJ_LIBRARY_GET_GLUT_H


// Unix
#ifdef unix
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#elif __unix
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#elif __unix__
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// Linux
#elif linux
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#elif __linux
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#elif __linux__
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// Mac OSX
#elif __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#elif __MACH__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>

// Solaris
#elif sun
#error "OpenGL includes for solaris"
#elif __sun
#error "OpenGL includes for solaris"

// FreeBSD
#elif __FreeBSD__
#error "OpenGL includes for FreeBSD"

// Windows
#elif _WIN32	// include 64-bit enviroments
#include "glut.h"
#elif __WIN32__
#include "glut.h"
#endif



#endif
