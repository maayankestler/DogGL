#include "Floor.h"

Floor::Floor(string texture_filename, GLfloat xMin, GLfloat xMax, GLfloat yMin, GLfloat yMax) {
	this->xMax = xMax;
	this->xMin = xMin;
	this->yMax = yMax;
	this->yMin = yMin;
	this->texture_id = ObjectGL::create_texture(texture_filename);
}

void Floor::draw()
{
	glPushMatrix();
	glNormal3d(0, 1, 0);

	// bind Texture
	glBindTexture(GL_TEXTURE_2D, this->texture_id);

	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(xMin, 0, yMin);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(xMax, 0, yMin);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(xMax, 0, yMax);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(xMin, 0, yMax);
	glEnd();

	glEnd();
	glPopMatrix();
}