#include <GL/glut.h>
#include <string>
#include <iostream>
#include <vector>
#include <limits>
#include <fstream>
#include <iostream>

using namespace std;

void DrawObject(string inputfile);

bool FileExists(const std::string& abs_filename);

string GetBaseDir(const std::string& filepath);

class ObjectGL {
	private:
		string inputfile;
	public:
		ObjectGL(string inputfile);
		void draw();
};