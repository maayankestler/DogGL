#include "Floor.h"

Floor::Floor(string texture_filename, GLfloat xMin, GLfloat xMax, GLfloat yMin, GLfloat yMax) {
	this->xMax = xMax;
	this->xMin = xMin;
	this->yMax = yMax;
	this->yMin = yMin;
	GLuint texture_id;
	int w, h;
	int comp;
	unsigned char* image = stbi_load(texture_filename.c_str(), &w, &h, &comp, STBI_default);
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (comp == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB,
			GL_UNSIGNED_BYTE, image);
	}
	else if (comp == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, image);
	}
	else {
		assert(0);  // TODO
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image);
	this->texture_id = texture_id;
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