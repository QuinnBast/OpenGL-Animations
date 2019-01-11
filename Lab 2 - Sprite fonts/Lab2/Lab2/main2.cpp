//
//  Main2.cpp
//

#include <string>

#include "GetGlut.h"
#include "Sleep.h"
#include "ObjLibrary/SpriteFont.h"
#include <sstream>

using namespace std;
using namespace ObjLibrary;

void initDisplay();
void keyboard(unsigned char key, int x, int y);
void update();
void reshape(int w, int h);
void display();

//Globals
SpriteFont font;

// Window size
int window_width = 640;
int window_height = 480;



int main(int argc, char* argv[])
{
	glutInitWindowSize(window_width, window_height);	// Generate window size
	glutInitWindowPosition(0, 0);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutCreateWindow("Sprite Fonts");
	glutKeyboardFunc(keyboard);
	glutIdleFunc(update);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);

	initDisplay();
	// load your font here
	font.load("Font.bmp");

	glutMainLoop();

	return 1;
}

void initDisplay()
{
	glClearColor(0.4f, 0.4f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: // on [ESC]
		exit(0); // normal exit
		break;
	}
}

void update()
{
	//update your variables here
	
	sleep(1.0 / 60.0);
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	// Update the variables when a window reshape event is executed
	window_width = w;
	window_height = h;

	glViewport (0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLdouble)w / (GLdouble)h, 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);

	glutPostRedisplay();
}

void display()
{
	// Allows conversion of numbers to text
	stringstream ss;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// clear the screen - any drawing before here will not display

	glLoadIdentity();
	gluLookAt(5.0, 3.0, 4.0,
	          0.0, 0.0, 0.0,
	          0.0, 1.0, 0.0);
	// set up the camera - any drawing before here will display incorrectly

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

	glColor3d(0.6, 0.4, 0.2);
	glutSolidSphere(1.0, 20, 10);

	// add your text drawing code here
	// Setup a font drawing canvas
	// The coordinates passed are not representative of the window size, and instead scale the passed cordinates as the edges of the screen it creates.
	// SpriteFont::setUp2dView(640, 480);

	// Can setup the font to draw respective to the window size. Will need to handle window resizing events to change the window size variables.
	// This will ensure that the font positions are absolutely positioned in the window and a window rescale will not change the position of the text.
	SpriteFont::setUp2dView(window_width, window_height);

	// Draw text at the specified coordinates
	// font.draw("Hello World!", 260, 220);

	// Can generate the font bolded
	font.draw("Hello World!", 250, 220, SpriteFont::BOLD);

	// Last three numbers are RGB to set the font color
	font.draw("Red Text", 385, 250, 255, 0, 0);
	font.draw("Yellow Text", 375, 190, 255, 255, 0);
	font.draw("Green Text", 275, 150, 0, 255, 0);
	font.draw("Cyan Text", 165, 190, 0, 255, 255);
	font.draw("Blue Text", 165, 250, 0, 0, 255);
	font.draw("Purple Text", 275, 300, 255, 0, 255, SpriteFont::ITALICS);

	// Write numbers to a string stream and they are converted to text:
	ss << "Numbers get converted! I am " << 23;
	string outputString = ss.str();
	font.draw(outputString, 50, 400, 0, 0, 0);

	float x = 5.9f;
	ss.str("");	// Reset the string stream
	ss << "Floats too: (" << x << ")";
	string anotherString = ss.str();
	font.draw(anotherString, 50, 450, 0, 0, 0);

	// Clear the drawing region
	SpriteFont::unsetUp2dView();

	// send the current image to the screen - any drawing after here will not display
	glutSwapBuffers();
}
