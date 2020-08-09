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
#include "Light.h"

// window vars
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 800;
const int WINDOW_POS_X = 400;
const int WINDOW_POS_Y = 150;
const float WINDOW_RATIO = WINDOW_WIDTH / (float)WINDOW_HEIGHT;
static float aspect = WINDOW_RATIO;

// camera vars
static GLfloat camera_position[3] = { 0, 10, 20 };
static GLfloat camera_target[3] = { 0, 0, 0 };
static GLfloat currentAngleOfRotation = 0.0;

// light
static float ambient_intensity = 0.2f;

// others
static bool show_coordinates = true;
static bool show_menu = true;


// imgui state
static bool show_demo_window = false;
static bool show_exit_window = false;
static void HelpMarker(const char* desc);

class Scene
{
private:	
	ObjectGL* dog;
	ObjectGL* statue;
	ObjectGL* table;
	Light* lamp;
	Floor* floor;
	void drawCoordinateArrows();
	static Scene* currentInstance;
	void display_menu();
public:
	Scene(int argc, char** argv);
	void display(); // Function where the scene drawing occures	
	void keyboard(unsigned char key, int x, int y); // Function for keyboard press
	void reshape(GLint w, GLint h); // Function to handle reshape of the screen
};

