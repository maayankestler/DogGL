#include <GL/glut.h>
#include <string>
#include <iostream>
#include <vector>
#include <limits>
#include <iostream>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
using namespace std;

// Constants
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 800;
const int WINDOW_POS_X = 400;
const int WINDOW_POS_Y = 150;
const double PI = 3.14159265358979323846;
const float WINDOW_RATIO = WINDOW_WIDTH / (float)WINDOW_HEIGHT;

string msg = "ortho";
float aspect = WINDOW_RATIO;
bool m_bShowCoordinateArrows = true;

// This is the number of frames per second to render.
static const int FPS = 60;

// This global variable keeps track of the current orientation of the polyhedron.
static GLfloat currentAngleOfRotation = 0.0;

// Set this to true to change projection.
static bool orthoProjection = true;

// Function where the scene drawing occures
void display(void);

// Function for keyboard press
void keyboard(unsigned char key, int x, int y);

// Function to handle reshape of the screen
void reshape(GLint w, GLint h);

// Function to handle the timer
void timer(int v);

// Function to update the projection
void updateProjection();

void DrawObject(string inputfile);

void drawCoordinateArrows(void);