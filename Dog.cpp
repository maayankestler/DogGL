#include "Dog.h"

Dog::Dog(string inputfile, GLfloat PosX, GLfloat PosY, GLfloat PosZ, GLfloat scale, glm::vec3 upVector, glm::vec3 towardVector, GLfloat angle) :
	ObjectGL(inputfile, PosX, PosY, PosZ, scale, upVector, towardVector, angle) {
	this->orignalSideVector = glm::cross(this->upVector, this->towardVector);
	this->sideVector = glm::vec3(orignalSideVector.x, orignalSideVector.y, orignalSideVector.z);
	string shape;
	for (size_t s = 0; s < this->shapes.size(); s++) {
		shape = this->shapes[s].name;
		if (shape.find("head") != std::string::npos) {
			this->addTask([this]() { handleRotation(DOG_HEAD); }, shape);
			continue;
		}
		if (shape.find("tail") != std::string::npos) {
			this->addTask([this]() { handleRotation(DOG_TAIL); }, shape);
			continue;
		}
		if (shape.find("leg") != std::string::npos) {
			if (shape.find("left") != std::string::npos) {
				if (shape.find("front") != std::string::npos) {
					this->addTask([this]() { handleRotation(DOG_LEFT_FRONT_LEG); }, shape);
					continue;
				}
				else if (shape.find("back") != std::string::npos) {
					this->addTask([this]() { handleRotation(DOG_LEFT_BACK_LEG); }, shape);
					continue;
				}
			} 
			else if (shape.find("right") != std::string::npos) {
				if (shape.find("front") != std::string::npos) {
					this->addTask([this]() { handleRotation(DOG_RIGHT_FRONT_LEG); }, shape);
					continue;
				}
				else if (shape.find("back") != std::string::npos) {
					this->addTask([this]() { handleRotation(DOG_RIGHT_BACK_LEG); }, shape);
					continue;
				}
			}
		}
	}
}

void Dog::rotateOrgan(GLfloat angle, int organ, bool vertical) {
	if (abs(organsAngles[organ][vertical] + angle) <= maxOrgansAngles[organ][vertical]) {
		organsAngles[organ][vertical] += angle;
	}
}

void Dog::handleRotation(int organ) {
	glTranslatef(organsPos[organ][0], organsPos[organ][1], organsPos[organ][2]);
	glRotatef(organsAngles[organ][false], this->upVector.x, this->upVector.y, this->upVector.z);
	glRotatef(organsAngles[organ][true], orignalSideVector.x, orignalSideVector.y, orignalSideVector.z);
	glTranslatef(-organsPos[organ][0], -organsPos[organ][1], -organsPos[organ][2]);
}

void Dog::walk(GLfloat distance) {
	moveLegs(legs_angle_per_step, true);
	ObjectGL::walk(distance);
}

void Dog::rotate(GLfloat angle) {
	moveLegs(legs_angle_per_rotate, false);
	ObjectGL::rotate(angle);
	this->sideVector = glm::cross(this->upVector, this->towardVector);
}

void Dog::wagTail(GLfloat angle) {
	if (abs(organsAngles[DOG_TAIL][false]) + angle > maxOrgansAngles[DOG_TAIL][false]) {
		tail_turn_right = !tail_turn_right;
	}
	if (tail_turn_right) {
		rotateOrgan(-angle, DOG_TAIL, false);
	}
	else {
		rotateOrgan(angle, DOG_TAIL, false);
	}
}

void Dog::moveLegs(GLfloat angle, bool vertical) {
	if (vertical) {
		if (abs(organsAngles[DOG_RIGHT_FRONT_LEG][vertical]) + angle > maxOrgansAngles[DOG_RIGHT_FRONT_LEG][vertical]) {
			right_legs_forward = !right_legs_forward;
		}
		if (right_legs_forward) {
			rotateOrgan(-angle, DOG_RIGHT_BACK_LEG, vertical);
			rotateOrgan(-angle, DOG_RIGHT_FRONT_LEG, vertical);
			rotateOrgan(angle, DOG_LEFT_BACK_LEG, vertical);
			rotateOrgan(angle, DOG_LEFT_FRONT_LEG, vertical);
		}
		else {
			rotateOrgan(angle, DOG_RIGHT_BACK_LEG, vertical);
			rotateOrgan(angle, DOG_RIGHT_FRONT_LEG, vertical);
			rotateOrgan(-angle, DOG_LEFT_BACK_LEG, vertical);
			rotateOrgan(-angle, DOG_LEFT_FRONT_LEG, vertical);
		}
	}
	else {
		if (abs(organsAngles[DOG_RIGHT_FRONT_LEG][vertical]) + angle > maxOrgansAngles[DOG_RIGHT_FRONT_LEG][vertical]) {
			legs_turn_right = !legs_turn_right;
		}
		if (legs_turn_right) {
			rotateOrgan(angle, DOG_RIGHT_BACK_LEG, vertical);
			rotateOrgan(angle, DOG_RIGHT_FRONT_LEG, vertical);
			rotateOrgan(angle, DOG_LEFT_BACK_LEG, vertical);
			rotateOrgan(angle, DOG_LEFT_FRONT_LEG, vertical);
		}
		else {
			rotateOrgan(-angle, DOG_RIGHT_BACK_LEG, vertical);
			rotateOrgan(-angle, DOG_RIGHT_FRONT_LEG, vertical);
			rotateOrgan(-angle, DOG_LEFT_BACK_LEG, vertical);
			rotateOrgan(-angle, DOG_LEFT_FRONT_LEG, vertical);
		}
	}
}

glm::vec3 Dog::getViewPos() {
	glm::vec3 eyes = glm::vec3(organsPos[DOG_EYES][0] / 2, organsPos[DOG_EYES][1] / 2, organsPos[DOG_EYES][2] / 2); // model eyes pos TODO change object and stop divide by half (or use scale vars)
	// start rotate by cur angle
	float rad_angle = (angle / 180) * glm::pi<float>(); // use radians
	glm::mat4 rotationMat(1);
	rotationMat = glm::rotate(rotationMat, rad_angle, this->upVector);
	eyes = glm::vec3(rotationMat * glm::vec4(eyes, 1.0));
	eyes += glm::vec3(PosX, PosY, PosZ); // add cur pos
	return eyes;
}

glm::vec3 Dog::getViewTarget() {
	glm::vec3 view_vector = getViewVector();
	glm::vec3 eyes = getViewPos();
	return eyes + view_vector;
}

glm::vec3 Dog::getViewVector() {
	glm::vec3 view_vector = towardVector;

	float rad_angle = (organsAngles[DOG_HEAD][true] / 180) * glm::pi<float>(); // use radians
	glm::mat4 rotationMat(1);
	rotationMat = glm::rotate(rotationMat, rad_angle, this->sideVector);
	view_vector = glm::vec3(rotationMat * glm::vec4(view_vector, 1.0));

	rad_angle = (organsAngles[DOG_HEAD][false] / 180) * glm::pi<float>();
	rotationMat = glm::rotate(rotationMat, rad_angle, this->upVector);
	view_vector = glm::vec3(rotationMat * glm::vec4(view_vector, 1.0));
	return view_vector;
}