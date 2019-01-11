#pragma once

#include "ObjLibrary/Vector2.h"
using namespace ObjLibrary;

class Square {

public:

	void update();
	void display();
	bool isAlive();
	void init(Vector2);
	Square();

private:

	Vector2 pos;
	Vector2 vel;
	double age;
	float red;
	float green;
	float blue;
	float size;
	float transparency;
	float rotation;
	float random0to1();


};