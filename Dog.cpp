#include "Dog.h"

Dog::Dog(string inputfile, GLfloat PosX, GLfloat PosY, GLfloat PosZ, glm::vec3 upVector, glm::vec3 towardVector, GLfloat angle) : 
	ObjectGL(inputfile, PosX, PosY, PosZ, upVector, towardVector, angle) {
	this->sideVector = glm::cross(this->upVector, this->towardVector);
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
	glRotatef(organsAngles[organ][true], sideVector.x, sideVector.y, sideVector.z);
	glTranslatef(-organsPos[organ][0], -organsPos[organ][1], -organsPos[organ][2]);
}

void Dog::walk(GLfloat distance) {
	moveLegs(legs_angle_per_step, true);
	ObjectGL::walk(distance);
}

void Dog::rotate(GLfloat angle) {
	moveLegs(legs_angle_per_rotate, false);
	ObjectGL::rotate(angle);
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