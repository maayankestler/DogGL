#pragma once

#include <GL/glut.h>
#include <string>
#include <iostream>
#include <vector>
#include <limits>
#include <fstream>
#include <iostream>
#include <tiny_obj_loader.h>
#include <stb_image.h>

using namespace std;

bool FileExists(const std::string& abs_filename);

string GetBaseDir(const std::string& filepath);

class ObjectGL {
	private:
		string inputfile;
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		GLuint texture_id;
	public:
		ObjectGL(string inputfile);
		// ObjectGL() {}; // default constructor
		void draw();
};