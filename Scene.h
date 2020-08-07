#pragma once

#include <GL/glut.h>
#include <string>
#include <iostream>
#include <vector>
#include <limits>
#include <fstream>
#include <iostream>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glut.h>
#include <imgui/imgui_impl_opengl2.h>

using namespace std;

#include "ObjectGL.h"
#include "Floor.h"

// Constants
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 800;
const int WINDOW_POS_X = 400;
const int WINDOW_POS_Y = 150;
const float WINDOW_RATIO = WINDOW_WIDTH / (float)WINDOW_HEIGHT;

// This is the number of frames per second to render.
static const int FPS = 60;

// This global variable keeps track of the current orientation of the polyhedron.
static GLfloat currentAngleOfRotation = 0.0;

void display_menu();

class Scene
{
private:	
	ObjectGL* dog;
	ObjectGL* statue;
	ObjectGL* table;
	ObjectGL* lamp;
	Floor* floor;
	void drawCoordinateArrows();
	static Scene* currentInstance;
public:
	Scene(int argc, char** argv);
	void display(); // Function where the scene drawing occures	
	void keyboard(unsigned char key, int x, int y); // Function for keyboard press
	void updateProjection(); // Function to update the projection
	void reshape(GLint w, GLint h); // Function to handle reshape of the screen
};

