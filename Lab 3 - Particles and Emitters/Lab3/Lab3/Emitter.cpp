#pragma once

#include "Emitter.h"
#include "ObjLibrary/Vector2.h"

using namespace ObjLibrary;
using namespace std;

template <class T>

Emitter::Emitter() {
	position.set(0.0, 0.0);
}

void Emitter::toggleEmitter() {
	on = !on;
}

void Emitter::spawnParticle() {
	particle_list.push_back(new T());
}

void Emitter::update() {

}
void Emitter::display() {

}