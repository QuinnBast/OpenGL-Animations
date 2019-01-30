//
//  Main4.cpp
//

#include <cassert>
#include <cmath>
#include <string>
#include <iostream>

#include "GetGlut.h"
#include "Sleep.h"
#include "ObjLibrary/ObjModel.h"
#include "ObjLibrary/DisplayList.h"

using namespace std;
using namespace ObjLibrary;

void init ();
void initDisplay ();
void keyboard (unsigned char key, int x, int y);
void update ();
void reshape (int w, int h);
void display ();

// add your global variables here
ObjModel spiky;
ObjModel bucket;
DisplayList bucket_list;
ObjModel skybox;



int main (int argc, char* argv[])
{
	glutInitWindowSize(1024, 768);
	glutInitWindowPosition(0, 0);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutCreateWindow("Loading OBJ Models");
	glutKeyboardFunc(keyboard);
	glutIdleFunc(update);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);

	init();

	glutMainLoop();

	return 1;
}

void init ()
{
	initDisplay();
	spiky.load("Spiky.obj");
	bucket.load("firebucket.obj");
	skybox.load("Skybox.obj");
	bucket_list = bucket.getDisplayList();
}

void initDisplay ()
{
	glClearColor(0.5, 0.5, 0.5, 0.0);
	glColor3f(0.0, 0.0, 0.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glutPostRedisplay();
}

void keyboard (unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: // on [ESC]
		exit(0); // normal exit
		break;
	}
}

void update ()
{
	// update your variables here
	
	sleep(1.0 / 60.0);
	glutPostRedisplay();
}

void reshape (int w, int h)
{
	glViewport (0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w / (GLdouble)h, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);

	glutPostRedisplay();
}

void display ()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(2.0, 1.0, 4.0,
	          0.0, 0.0, 0.0,
	          0.0, 1.0, 0.0);
	glPushMatrix();
		glTranslated(2.0, 1.0, 4.0);
		glScaled(600.0, 600.0, 600.0);
		glDepthMask(GL_FALSE);
		skybox.draw();
		glDepthMask(GL_TRUE);
	glPopMatrix();

	// display positive X, Y, and Z axes near origin
	glBegin(GL_LINES);
		glColor3d(1.0, 0.0, 0.0);
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(2.0, 0.0, 0.0);
		glColor3d(0.0, 1.0, 0.0);
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(0.0, 2.0, 0.0);
		glColor3d(0.0, 0.0, 1.0);
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(0.0, 0.0, 2.0);
	glEnd();

	// draw a purple wireframe cube
	glColor3d(1.0, 0.0, 1.0);
	glPushMatrix();
		glTranslated(0.0, 0.0, 0.0);
		glRotated(45, 0.0, 1.0, 0.0);
		glScaled(1.0, 1.0, 1.0);
		glutWireCube(1.0);
	glPopMatrix();

	// Draw the loaded model
	// glColor3d(1.0, 0.0, 0.0);
	// spiky.draw();

	// Loop and create multiple spikys
	for (int i = 0; i < 50; i++) {
		glPushMatrix();
			glTranslatef(1.0 + i, 0, 0);
			glScaled(0.45, 0.45, 0.45);
			spiky.draw();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-1.0 + -i, 0, 0);
			glScaled(0.005, 0.005, 0.005);
			bucket.draw();
		glPopMatrix();
	}

	glutSwapBuffers();
}
