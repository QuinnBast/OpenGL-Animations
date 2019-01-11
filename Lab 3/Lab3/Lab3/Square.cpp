#pragma once

#include "Square.h"
#include "ObjLibrary/Vector2.h"
#include "GetGlut.h"
#include <cstdlib>
using namespace ObjLibrary;

Square::Square() {
	pos = Vector2(0.0, 0.0);
	vel = Vector2(5.0, 1.0);
	age = 999999;
	transparency = 1;
	red = random0to1();
	green = random0to1();
	blue = random0to1();
	size = 20 + (random0to1() * 30);
}

void Square::init(Vector2 initialPosition)
{
	pos = initialPosition;
	vel = Vector2::getRandomUnitVector() * 3.0;
	age = 0;

	// Random color particles
	// red = random0to1();
	// green = random0to1();
	// blue = random0to1();

	// Ambient orange
	red = 1.0;
	green = 0.3;
	blue = 0.1;

	rotation = random0to1() * 360;
	
	// Random radius from middle to edge
	size = (20 + (random0to1() * 30));
	transparency = 1;
}

void Square::update() {
	// update your variables here
	pos += vel;
	transparency = 1 - (age / 60);
	rotation += 1;
	age++;
}

void Square::display() {
	glPushMatrix(); // Copy the top matrix
// Set the square's position.

	glTranslated(pos.x, pos.y, 0.0f);
	glRotatef(rotation, 0.0f, 0.0f, 1.0f);

	// Make squares transparent
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Set transparency type
		// Additive transparency, makes things brighter where overlaps are more severe.
		// Makes things like fire, explosions, etc. stand out.
		// Works best on dark backgrounds
		
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// Uses alpha transparency, simply overlaps the transparencies and displays what is behind

	// Set square color
	glColor4f(red, green, blue, transparency);
	glBegin(GL_POLYGON);

	//Draw the square vertices
	/* 
	glVertex2f(-size, -size);
	glVertex2f(-size, size);
	glVertex2f(size, size);
	glVertex2f(size, -size);
	*/

	// Draw as an octagon

	//Left side
	glVertex2f(-size, -size*(1.0/2.0));
	glVertex2f(-size, size*(1.0 / 2.0));
	// Left to top
	glVertex2f(-size*(1.0 / 2.0), size);
	//Top side
	glVertex2f(size*(1.0 / 2.0), size);
	//Top to right
	glVertex2f(size, size*(1.0 / 2.0));
	//Right
	glVertex2f(size, -size*(1.0 / 2.0));
	//Right to bottom
	glVertex2f(size*(1.0 / 2.0), -size);
	//Bottom
	glVertex2f(-size * (1.0 / 2.0), -size);

	glEnd();
	glDisable(GL_BLEND);

	glPopMatrix();	// Draw the top matrix
}

bool Square::isAlive()
{
	if (age <= 60)
		return true;
	else
		return false;
}

float Square::random0to1()
{
	return (float)(rand()) / ((float)RAND_MAX + 1.0f);
}