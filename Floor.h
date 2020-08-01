#pragma once
#include <GL/glut.h>
#include <string>
#include <iostream>     
#include <cassert>
#include <stb_image.h>
#include "ObjectGL.h"

using namespace std;

class Floor
{
public:
	Floor(string texture_filename, GLfloat xMin, GLfloat xMax, GLfloat yMin, GLfloat yMax);
	void draw();
	~Floor() = default;
	GLfloat xMin;
	GLfloat xMax;
	GLfloat yMin;
	GLfloat yMax;
private:
	GLuint texture_id;
};