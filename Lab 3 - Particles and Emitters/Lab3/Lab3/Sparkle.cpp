#pragma once

#include "Sparkle.h"
#include "ObjLibrary/Vector2.h"
#include "GetGlut.h"
#include <cstdlib>
using namespace ObjLibrary;

Sparkle::Sparkle() {
	pos = Vector2(random0to1() * 40.0, random0to1() * 40.0);
	age = 999999;
	transparency = 1;
	red = 1.0;
	green = (233.0/255.0);
	blue = 0.0;
	size = 10 + (random0to1() * 10);
}

void Sparkle::init(Vector2 initialPosition)
{
	pos = Vector2(initialPosition.x + random0to1() * 150.0, initialPosition.y + random0to1() * 150.0);
	age = 0;

	rotation = random0to1() * 360;

	// Random radius from middle to edge
	size = 10.0f + (random0to1() * 10.0f);
	transparency = 1;
}

void Sparkle::update() {
	size = 10 * (1 - (age / 60.0f));
	rotation += 0.4f;
	age++;
}

void Sparkle::display() {
	glPushMatrix(); // Copy the top matrix
// Set the square's position.

	glTranslated(pos.x, pos.y, 0.0f);
	glRotatef(rotation, 0.0f, 0.0f, 1.0f);

	// Make squares transparent
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set color
	glColor4f(red, green, blue, transparency);
	glBegin(GL_TRIANGLES);

		// Draw a star
		//Top Point
		glVertex2f(0.0, size);
		glVertex2f(-size * (0.1), 0);
		glVertex2f(size * (0.1), 0.0);

		//Right Point
		glVertex2f(-size, 0.0);
		glVertex2f(0.0, -size * (0.1));
		glVertex2f(0.0, size * (0.1));

		//Bottom Point
		glVertex2f(0.0, -size);
		glVertex2f(size * (0.1), 0.0);
		glVertex2f(-size * (0.1), 0.0);

		//Left Point
		glVertex2f(size, 0.0);
		glVertex2f(0.0, size * (0.1));
		glVertex2f(0.0, -size * (0.1));

	glEnd();
	glDisable(GL_BLEND);

	glPopMatrix();	// Draw the top matrix
}

bool Sparkle::isAlive()
{
	return (age <= 60);
}

float Sparkle::random0to1()
{
	return (float)(rand()) / ((float)RAND_MAX + 1.0f);
}