#include "Walls.h"

Walls::Walls(GLfloat height, GLfloat xMin, GLfloat xMax, GLfloat yMin, GLfloat yMax) {
	this->xMin = xMin;
	this->xMax = xMax;
	this->yMin = yMin;
	this->yMax = yMax;
	this->height = height;
}

void Walls::draw() {
	glPushMatrix();
	glNormal3d(0, 1, 0);

	// bind Texture
	//glBindTexture(GL_TEXTURE_2D, this->texture_id);

	GLfloat specular[] = { 1.0f, 1.0f, 1.0f };
	GLfloat shininess = 128.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	GLfloat wall_color[4] = { this->color[0], this->color[1], this->color[2], this->alpha };

	glBegin(GL_QUADS);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, wall_color);
		if (showSouth) {
			glNormal3d(0, 0, -1);
			//glTexCoord2f(0.0, 0.0);
			glVertex3f(xMin, 0, yMin);
			//glTexCoord2f(1.0, 0.0);
			glVertex3f(xMin, height, yMin);
			//glTexCoord2f(0.0, 1.0);
			glVertex3f(xMax, height, yMin);
			//glTexCoord2f(1.0, 1.0);
			glVertex3f(xMax, 0, yMin);
		}
		if (showNorth) {
			glNormal3d(0, 0, 1);
			glVertex3f(xMin, 0, yMax);
			glVertex3f(xMin, height, yMax);
			glVertex3f(xMax, height, yMax);
			glVertex3f(xMax, 0, yMax);
		}
		if (showWest) {
			glNormal3d(1, 0, 0);
			glVertex3f(xMin, 0, yMin);
			glVertex3f(xMin, height, yMin);
			glVertex3f(xMin, height, yMax);
			glVertex3f(xMin, 0, yMax);
		}
		if (showEast) {
			glNormal3d(-1, 0, 0);
			glVertex3f(xMax, 0, yMin);
			glVertex3f(xMax, height, yMin);
			glVertex3f(xMax, height, yMax);
			glVertex3f(xMax, 0, yMax);
		}
	glEnd();

	glPopMatrix();
}