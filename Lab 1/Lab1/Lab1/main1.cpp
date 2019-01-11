//
//  main1.cpp
//

#include "glut.h"
#include "Sleep.h"

// function prototypes
void display ();
void idle();
void keyboard(unsigned char key, int x, int y);
void special(int special_key, int x, int y);

//declare global variables here
float theta = 0.0f;
float thetaIncrement = 2.0f;
float xPosition = 0.0f;
float xIncrement = 0.01f;
float yPosition = 0.75f;
float yIncrement = 0.01f;


int main (int argc, char** argv)
{
	// initialize OpenGL; pass it the command line arguments
	glutInit(&argc, argv);	
	
	glutCreateWindow("First OpenGL Program");
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMainLoop();
	return 1;
}

void special(int special_key, int x, int y)
{
	switch (special_key)
	{
	case GLUT_KEY_LEFT:
		xPosition -= xIncrement;
		break;
	case GLUT_KEY_RIGHT:
		xPosition += xIncrement;
		break;
	}
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'R':
	case 'r':
		xPosition = 0.0;
		break;
	case 27: // on [ESC]
		exit(0); // normal exit
		break;
	case 'a':
	case 'A':
		xPosition -= xIncrement;
		// Update rotation angle
		theta -= 2.0f;
		if (theta <= 0.0f)
			theta += 360.0f;
		break;
	case 'D':
	case 'd':
		xPosition += xIncrement;
		// Update rotation angle
		theta += 2.0f;
		if (theta >= 360.0f)
			theta -= 360.0f;
		break;
	case 'W':
	case 'w':
		yPosition += yIncrement;
		break;
	case 'S':
	case 's':
		yPosition -= yIncrement;
		break;
	}
}

void idle() {
	sleep(0.01); // wait for 0.01 seconds

	// Redisplay as often as possible
	glutPostRedisplay();

	// Move across the screen
	// xPosition += xIncrement;
	// if (xPosition > 1.0f || xPosition < -1.0f)
		// xIncrement = -xIncrement;

}

void display (void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();	// copies existing top matrix
	glTranslatef(xPosition, yPosition, 0.0f); // Translate the position
	glScalef(0.25f, 0.25f, 0.25f);		// Scale the vector
	glRotatef(theta, 0.0f, 0.0f, 1.0f);	// Rotate the image by theta

		// Set the color of the rectangle via unsigned byte
		glColor3ub(0, 255, 0);	// unsigned byte

		glBegin(GL_POLYGON);

		// Generate a triangle
		// glVertex2f(-1, 1);
		// glVertex2f(1, 1);
		// glVertex2f(0, -1);

		glEnd();

		// Set the color of the rectangle via float:
		// glColor3f(1.0f, 0.5f, 0.0f);	// Orange

		// Set the color of the rectangle via double:
		// glColor3d(1.0, 0.5, 0.0);	// double

		// Set the color of the rectangle via unsigned byte
		glColor3ub(255, 128, 0);	// unsigned byte

		glBegin(GL_POLYGON);
			// Generate a cube:
		/*
			glVertex2f(-0.5f, -0.5f);
			glVertex2f( 0.5f, -0.5f);
			glVertex2f( 0.5f,  0.5f);
			glVertex2f(-0.5f,  0.5f);
		*/

			// Generate a rectangle:
		/*
			glVertex2f(-0.3f, -0.5f);
			glVertex2f(0.5f, -0.5f);
			glVertex2f(0.5f, 0.5f);
			glVertex2f(-0.3f, 0.5f);
		*/

			// Generate a smaller rectangle:

			// glVertex2f(0.0f, -0.5f);
			// glVertex2f(0.5f, -0.5f);
			// glVertex2f(0.5f, 0.5f);
			// glVertex2f(0.0f, 0.5f);

		glEnd();

		glColor3ub(204, 102, 0);	// unsigned byte

		glBegin(GL_POLYGON);

		// Generate an octogon

		//Left side
		glVertex2f(-1.0, -1.0 * (1.0 / 2.0));
		glVertex2f(-1.0, 1.0*(1.0 / 2.0));
		// Left to top
		glVertex2f(-1.0 * (1.0 / 2.0), 1.0);
		//Top side
		glVertex2f(1.0*(1.0 / 2.0), 1.0);
		//Top to right
		glVertex2f(1.0, 1.0*(1.0 / 2.0));
		//Right
		glVertex2f(1.0, -1.0 * (1.0 / 2.0));
		//Right to bottom
		glVertex2f(1.0*(1.0 / 2.0), -1.0);
		//Bottom
		glVertex2f(-1.0 * (1.0 / 2.0), -1.0);

		glEnd();

		glColor3ub(255, 0, 128);	// unsigned byte

		glBegin(GL_POLYGON);

			// Generate a hexagon

			glVertex2f(-1.0, 0.0);
			glVertex2f(-1.0*(1.0/2.0), 1.0);
			glVertex2f(1.0*(1.0 / 2.0), 1.0);
			glVertex2f(1.0, 0.0);
			glVertex2f(1.0*(1.0 / 2.0), -1.0);
			glVertex2f(-1.0*(1.0 / 2.0), -1.0);

		glEnd();

	glPopMatrix();

	// ensure that all buffered OpenGL commands get done
	glFlush();	
}
