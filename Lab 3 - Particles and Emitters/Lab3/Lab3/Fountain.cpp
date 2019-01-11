#pragma once

#include "Fountain.h"
#include "ObjLibrary/Vector2.h"
#include "GetGlut.h"
#include <cstdlib>
using namespace ObjLibrary;

Fountain::Fountain() {
	pos = Vector2(-1.0, 0.0);
	if (random0to1() > 0.5f) {
		vel = Vector2(random0to1() * 6.0, random0to1() * 50.0);
	}
	else {
		vel = Vector2(random0to1() * -6.0, random0to1() * 50.0);
	}
	age = 999999;
	transparency = 1;
	red = 0.0;
	green = 0.7;
	blue = 0.5;
	size = 2 + (random0to1() * 10);
}

void Fountain::init(Vector2 initialPosition)
{
	pos = initialPosition;
	if (random0to1() > 0.5f) {
		vel = Vector2(random0to1() * 2.0, 15 + random0to1() * 15.0);
	}
	else {
		vel = Vector2(random0to1() * -2.0, 15 + random0to1() * 15.0);
	}
	age = 0;

	rotation = random0to1() * 360;

	// Random radius from middle to edge
	size = 2 + (random0to1() * 10);
	transparency = 1;
}

void Fountain::update() {
	pos += vel;

	if (vel.y > -9.0) {
		vel += Vector2(0.0, -1.0);
	}

	if (vel.y < 10) {
		blue = (255.0 - (vel.y * 5.0)) / 255.0;
	}

	if (age > 25) {
		transparency = (25 - age) / 25;
	}

	rotation += 1;
	age++;
}

void Fountain::display() {
	glPushMatrix(); // Copy the top matrix
// Set the square's position.

	glTranslated(pos.x, pos.y, 0.0f);
	glRotatef(rotation, 0.0f, 0.0f, 1.0f);

	// Make squares transparent
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Set transparency type
		// Additive transparency, makes things brighter where overlaps are more severe.

	// Set color
	glColor4f(red, green, blue, transparency);
	glBegin(GL_POLYGON);

	// Draw as an octagon

	//Left side
	glVertex2f(-size, -size * (1.0 / 2.0));
	glVertex2f(-size, size*(1.0 / 2.0));
	// Left to top
	glVertex2f(-size * (1.0 / 2.0), size);
	//Top side
	glVertex2f(size*(1.0 / 2.0), size);
	//Top to right
	glVertex2f(size, size*(1.0 / 2.0));
	//Right
	glVertex2f(size, -size * (1.0 / 2.0));
	//Right to bottom
	glVertex2f(size*(1.0 / 2.0), -size);
	//Bottom
	glVertex2f(-size * (1.0 / 2.0), -size);

	glEnd();
	glDisable(GL_BLEND);

	glPopMatrix();	// Draw the top matrix
}

bool Fountain::isAlive()
{
	return (age <= 35);
}

float Fountain::random0to1()
{
	return (float)(rand()) / ((float)RAND_MAX + 1.0f);
}