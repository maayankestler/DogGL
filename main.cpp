#include "main.h"

// Initializes GLUT, the display mode, and main window; registers callbacks;
// enters the main event loop.
int main(int argc, char** argv) {	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(400, 150);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("dog world");
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutTimerFunc(100, timer, 0);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
}

// Handles the window reshape event
void reshape(GLint w, GLint h) {
	glViewport(0, 0, w, h);
	aspect = float(w / h);
	updateProjection();
}

// Handles the display callback to show what we have drawn.
void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -20);
	//glScalef(1, 1, 1);
	glRotatef(15, 1.0, 1.0, 1.0);
	glRotatef(currentAngleOfRotation, 0.0, 1.0, 0.0);

	// draw object
	/*string inputfile = "Retriver2.obj";
	DrawObject(inputfile);*/
	ObjectGL dog = ObjectGL::ObjectGL("Retriver2.obj");
	dog.draw();

	glRasterPos3f(0.0, 6.0, 0.0);
	for (char& c : msg) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}

	drawCoordinateArrows();
	glFlush();
	glutSwapBuffers();
}

// Handles the timer by incrementing the angle of rotation and requesting the
// window to display again.
// Since the timer function is only called once, it sets the same function to
// be called again.
void timer(int v) {
	currentAngleOfRotation += 1.0;
	if (currentAngleOfRotation > 360.0) {
		currentAngleOfRotation -= 360.0;
	}
	glutPostRedisplay();
	glutTimerFunc(1000 / FPS, timer, v);
}

// Handles keyboard press
void keyboard(unsigned char key, int x, int y) {
	orthoProjection = !orthoProjection;
	updateProjection();
}

void updateProjection() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (orthoProjection) {
		glOrtho(-10.0, 10.0, -10.0, 10.0, -30.0, 30.0);
		msg = "ortho";
		//glColor3f(0.2, 1.0, 0.2);
	}
	else {
		gluPerspective(60.0, aspect, 1, 100.0);
		msg = "Perspective";
		//glColor3f(0.0, 0.0, 0.8);
	}
	glutPostRedisplay();
}

void drawCoordinateArrows(void) {
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