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

const string OBJECTS_DIR = "objects";
const string TEXTURES_DIR = "textures";

class ObjectGL {
	private:
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		map<string, GLuint> textures;
	public:
		ObjectGL(string inputfile, GLfloat PosX = 0, GLfloat PosY = 0, GLfloat PosZ = 0, 
			     glm::vec3 upVector = glm::vec3(0, 1, 0), glm::vec3 towardVector = glm::vec3(0, 0, 0), GLfloat angle = 0);
		string inputfile;
		GLfloat PosX;
		GLfloat PosZ;
		GLfloat PosY;
		GLfloat angle;
		glm::vec3 towardVector;
		glm::vec3 upVector;
		map<string, vector<function<void()>>> shapesTasks;
		void draw();
		void setPosition(GLfloat x, GLfloat y, GLfloat z);
		void rotate(GLfloat angle);
		void addTask(function<void()> func, string shape = "GLOBAL");
		void walk(float distance);
		static GLuint create_texture(string texture_filename);
};