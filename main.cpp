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
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -20);
	//glScalef(1, 1, 1);
	glRotatef(15, 1.0, 1.0, 1.0);
	glRotatef(currentAngleOfRotation, 0.0, 1.0, 0.0);
	/*glLineWidth(4.0);
	glutWireDodecahedron();*/		

	// draw object
	string inputfile = "Retriver2.obj";
	DrawObject(inputfile);

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

void DrawObject(string inputfile) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());

	if (!warn.empty()) {
		std::cout << warn << std::endl;
	}

	if (!err.empty()) {
		std::cerr << err << std::endl;
	}

	if (!ret) {
		exit(1);
	}

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			int fv = shapes[s].mesh.num_face_vertices[f];

			glBegin(GL_POLYGON);
			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
				tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
				tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
				tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
				tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
				tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
				tinyobj::real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
				tinyobj::real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];
				// Optional: vertex colors
				tinyobj::real_t red = attrib.colors[3 * idx.vertex_index + 0];
				tinyobj::real_t green = attrib.colors[3 * idx.vertex_index + 1];
				tinyobj::real_t blue = attrib.colors[3 * idx.vertex_index + 2];

				// glColor3f(red, green, blue);
				//std::cout << vx << vy << vz << red << green << blue << std::endl;
				glColor3f(red, green, blue);
				glVertex3f(vx, vy, vz);
			}
			index_offset += fv;
			glEnd();
			// per-face material
			// shapes[s].mesh.material_ids[f];
		}

	}
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

	glRasterPos3f(1.2, 0.0, 0.0);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'x');
	glRasterPos3f(0.0, 1.2, 0.0);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'y');
	glRasterPos3f(0.0, 0.0, 1.2);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'z');
}