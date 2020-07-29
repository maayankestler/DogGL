#pragma once

#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <limits>
#include <fstream>
#include <tiny_obj_loader.h>
#include <stb_image.h>
#include <functional>
#include <vector>
#include <map>

using namespace std;

bool FileExists(const std::string& abs_filename);

string GetBaseDir(const std::string& filepath);

const double PI = atan(1) * 4; // 3.14159265358979323846 https://stackoverflow.com/questions/1727881/how-to-use-the-pi-constant-in-c

class ObjectGL {
	private:
		// queue <function<void()>> tasksQueue;
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		map<string, GLuint> textures;
	public:
		ObjectGL(string inputfile, GLfloat PosX = 0, GLfloat PosY = 0, GLfloat PosZ = 0);
		// ObjectGL() {};
		string inputfile;
		GLfloat PosX;
		GLfloat PosZ;
		GLfloat PosY;
		GLfloat angle;
		glm::vec3 towardVector;
		glm::vec3 upVector;
		vector<function<void()>> tasks;
		void draw();
		void setPosition(GLfloat x, GLfloat y, GLfloat z);
		void rotate(GLfloat angle);
		void addTask(function<void()> func);
		void walk(float distance);
};