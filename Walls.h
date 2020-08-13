#pragma once
#include <GL/glut.h>
#include <string>
// #include "ObjectGL.h"

using namespace std;

class Walls
{
public:
	Walls(GLfloat height, GLfloat xMin=-10, GLfloat xMax=10, GLfloat yMin=-10, GLfloat yMax=10);
	void draw();
	~Walls() = default;
	GLfloat xMin;
	GLfloat xMax;
	GLfloat yMin;
	GLfloat yMax;
	GLfloat height;
	GLfloat alpha = 0.5f;
	GLfloat color[3] = { 0.5f, 0.78f, 0.884f };
	bool showNorth = false;
	bool showSouth = true;
	bool showEast = true;
	bool showWest = true;
private:
	// GLuint texture_id;
};