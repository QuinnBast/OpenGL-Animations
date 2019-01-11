#pragma once

#include "ObjLibrary/Vector2.h"
#include "vector"
using namespace ObjLibrary;
using namespace std;

template <class T>
class Emitter {

public:
	void toggleEmitter();
	void spawnParticle();
	void update();
	void display();

private:
	bool on;
	int frequency;
	Vector2 position;
	std::vector<T> paricle_list;

};