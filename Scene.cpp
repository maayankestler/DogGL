#include "Scene.h"

float aspect = WINDOW_RATIO;
bool m_bShowCoordinateArrows = true;

// creating callbacks to the class functions as describe in 
// https://stackoverflow.com/questions/3589422/using-opengl-glutdisplayfunc-within-class
Scene* currentInstance;
void displaycallback()
{
	currentInstance->display();
}
void reshapecallback(GLint w, GLint h)
{
	currentInstance->reshape(w, h);
}
void timercallback(int v)
{
	currentInstance->timer(v);
}
void keyboardcallback(unsigned char key, int x, int y)
{
	currentInstance->keyboard(key, x, y);
}

// Initializes GLUT, the display mode, and main window; registers callbacks;
// enters the main event loop.
Scene::Scene(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(WINDOW_POS_X, WINDOW_POS_Y);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("dog world");
	this->dog = new ObjectGL("GermanShephardLowPoly2.obj", 0, 0, 0);
	// this->dog = new ObjectGL("GermanShephardLowPoly.obj", 0, 0, 0);
	this->dog->towardVector = glm::vec3(-1, 0, 0);
	this->dog->upVector = glm::vec3(0, 1, 0);
	this->dog->addTask([]() { glScalef(0.5f, 0.5f, 0.5f); });
	this->floor = new Floor("floor.jpg", -10, 10, -10, 10);
	this->statue = new ObjectGL("venus_polygonal_statue.obj", -30, 0, -30);
	this->statue->addTask([]() { glScalef(0.07f, 0.07f, 0.07f); });
	this->table = new ObjectGL("abciuppa_table_w_m_01.obj", 2, 0, 2);
	this->table->addTask([]() { glScalef(3.0f, 3.0f, 3.0f); });

	::currentInstance = this;
	glutReshapeFunc(reshapecallback);
	glutDisplayFunc(displaycallback);
	// glutTimerFunc(100, timercallback, 0);
	glutKeyboardFunc(keyboardcallback);
	glutMainLoop();
}

// Handles the window reshape event
void Scene::reshape(GLint w, GLint h) {
	glViewport(0, 0, w, h);
	aspect = float(w / h);
	updateProjection();
}

// Handles the display callback to show what we have drawn.
void Scene::display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// start drawing
	// glRotatef(currentAngleOfRotation, 0.0, 1.0, 0.0);
	floor->draw();
	statue->draw();
	table->draw();
	dog->draw();
	// add Coordinate Arrows for debug
	drawCoordinateArrows();
	glFlush();
	glutSwapBuffers();
}

// Handles the timer by incrementing the angle of rotation and requesting the
// window to display again.
// Since the timer function is only called once, it sets the same function to
// be called again.
void Scene::timer(int v) {
	currentAngleOfRotation += 1.0;
	if (currentAngleOfRotation > 360.0) {
		currentAngleOfRotation -= 360.0;
	}
	glutPostRedisplay();
	glutTimerFunc(1000 / FPS, timercallback, v);
}

// Handles keyboard press
void Scene::keyboard(unsigned char key, int x, int y) {
	key = tolower(key);
	if (key == 'w') {
		dog->walk(1.0f);
	}
	else if (key == 's') {
		dog->walk(-1.0f);
	}
	else if (key == 'd') {
		dog->rotate(-5.0f);
	}
	else if (key == 'a') {
		dog->rotate(5.0f);
	}
	//else if (key == 'o') {
	//	dog->setPosition(dog->PosX, dog->PosY + 1, dog->PosZ);
	//	std::cout << dog->PosX << dog->PosY << dog->PosZ << std::endl;
	//}
	//else if (key == 'l') {
	//	dog->setPosition(dog->PosX, dog->PosY - 1, dog->PosZ);
	//	std::cout << dog->PosX << dog->PosY << dog->PosZ << std::endl;
	//}
	glutPostRedisplay();
}

void Scene::updateProjection() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, aspect, 1, 100.0);
	gluLookAt(0, 10, 20,
		      0, 0, 0,
		      0, 1, 0);
	glutPostRedisplay();
}

void Scene::drawCoordinateArrows(void) {
	if (!m_bShowCoordinateArrows) {
		return;
	}

	glColor3f(1.0, 0.0, 0.0);

	glBegin(GL_LINE_STRIP);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(1.0, 0.0, 0.0);
	glEnd();


	glBegin(GL_LINE_STRIP);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 1.0, 0.0);
	glEnd();


	glBegin(GL_LINE_STRIP);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 1.0);
	glEnd();

	glRasterPos3f(1.2f, 0.0f, 0.0f);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'x');
	glRasterPos3f(0.0f, 1.2f, 0.0f);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'y');
	glRasterPos3f(0.0f, 0.0f, 1.2f);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'z');
}