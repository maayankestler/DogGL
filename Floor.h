#pragma once
#include <GL/glut.h>
#include <string>
#include <iostream>     
#include <cassert>
#include <stb_image.h>
// #include "ObjectGL.h"

using namespace std;

class Floor
{
public:
	Floor(GLfloat xMin, GLfloat xMax, GLfloat yMin, GLfloat yMax, int rows = 10, int columns = 10);
	void draw();
	~Floor() = default;
	GLfloat xMin;
	GLfloat xMax;
	GLfloat yMin;
	GLfloat yMax;
	GLfloat color1[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat color2[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
	int rows;
	int columns;
private:
	// GLuint texture_id;
};