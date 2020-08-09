#include "Light.h"

Light::Light(int id, GLfloat PosX, GLfloat PosY, GLfloat PosZ, string object,
		     GLfloat cutoff, GLfloat exponent,
	         GLfloat TargetX, GLfloat TargetY, GLfloat TargetZ) {
	this->id = id;
	this->position[0] = PosX;
	this->position[1] = PosY;
	this->position[2] = PosZ;
	this->position[3] = 1.0f;
	this->target[0] = TargetX;
	this->target[1] = TargetY;
	this->target[2] = TargetZ;
	this->cutoff = cutoff;
	this->exponent = exponent;

	if (object.length() > 0) {
		this->lamp = new ObjectGL(object);
	}

	enable();
}

void Light::draw() {
	if (!glIsEnabled(id))
		return;

	glPushMatrix();
	glTranslatef(position[0], position[1], position[2]);
	fixDirection();

	if (this->lamp != NULL) {
		this->lamp->draw();
	}
	else
	{
		GLfloat ambient[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
		GLfloat diffuse[4] = { 0.01f, 0.01f, 0.01f, 1.0f };
		GLfloat specular[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
		GLfloat shininess = 32.0f;

		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);

		glutSolidCone(0.3, 0.6, 10, 10);
		glPushMatrix();
		glTranslatef(0, 0, 0.1f);
		glutSolidCylinder(0.2, 0.39, 10, 10);
		glPopMatrix();

		glDisable(GL_LIGHTING);
		glColor3fv(color);
		glutSolidSphere(0.2, 100, 100);
		glEnable(GL_LIGHTING);
	}
	
	glPopMatrix();
}

void Light::disable()
{
	glDisable(id);
}

void Light::enable()
{
	glEnable(id);
}

void Light::addlight() {
	if (!glIsEnabled(id))
		return;
	glLightfv(id, GL_DIFFUSE, this->color);
	glLightfv(id, GL_SPECULAR, this->color);
	glLightfv(id, GL_POSITION, this->position);
	GLfloat direction[3] = { this->target[0] - this->position[0],
							 this->target[1] - this->position[1],
							 this->target[2] - this->position[2] };
	glLightfv(this->id, GL_SPOT_DIRECTION, direction);
	glLightf(this->id, GL_SPOT_CUTOFF, this->cutoff);
	glLightf(this->id, GL_SPOT_EXPONENT, this->exponent);
}

void Light::fixDirection() {
	glm::vec3 eye = glm::vec3(this->position[0], this->position[1], this->position[2]);
	glm::vec3 center = glm::vec3(this->target[0], this->target[1], this->target[2]);

	glm::vec3 wantedVector = glm::normalize(center - eye);
	glm::vec3 normal = glm::cross(towardVector, wantedVector);
	normal = glm::normalize(normal);
	float radian_angle = glm::angle(towardVector, wantedVector);
	float angle = radian_angle * 180 / glm::pi<float>();
	glRotatef(angle, normal.x, normal.y, normal.z);
}