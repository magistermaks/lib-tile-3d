
#include "scene.hpp"

void Scene::set( int offset, float a, float b, float c ) {
	float* row = this->buffer + (offset * 3);
	row[0] = a;
	row[1] = b;
	row[2] = c;
}

void Scene::setCameraOrigin( float a, float b, float c ) {
	this->set(0, a, b, c);
}

void Scene::setCameraDirection( float a, float b, float c ) {
	this->set(1, a, b, c);
}

void Scene::setSkyLight( float a, float b, float c ) {
	this->set(2, a, b, c);
}

void Scene::setAmbientLight( float a, float b, float c ) {
	this->set(3, a, b, c);
}

void Scene::setBackground( float a, float b, float c ) {
	this->set(4, a, b, c);
}

void Scene::setProjection( float a, float b, float c ) {
	this->set(5, a, b, c);
}

float* Scene::ptr() {
	return this->buffer;
}

size_t Scene::size() {
	return sizeof(this->buffer);
}


