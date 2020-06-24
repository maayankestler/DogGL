#include "ObjectGL.h"
#ifndef TINYOBJLOADER_IMPLEMENTATION
	#define TINYOBJLOADER_IMPLEMENTATION
	#include <tiny_obj_loader.h>
#endif
#ifndef STB_IMAGE_IMPLEMENTATION
	#define STB_IMAGE_IMPLEMENTATION
	#include <stb_image.h>
#endif

ObjectGL::ObjectGL(string inputfile) {
	this->inputfile = inputfile;
}


void ObjectGL::draw() {
	DrawObject(inputfile);
}

void DrawObject(string inputfile) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());

	if (!warn.empty()) {
		std::cout << warn << std::endl;
	}

	if (!err.empty()) {
		std::cerr << err << std::endl;
	}

	if (!ret) {
		exit(1);
	}

	// create texture
	GLuint texture_id;
	int w, h;
	int comp;
	string texture_filename = materials[0].diffuse_texname;
	if (!FileExists(texture_filename)) {
		string base_dir = GetBaseDir(inputfile);
		// Append base dir.
		texture_filename = base_dir + texture_filename;
		if (!FileExists(texture_filename)) {
			std::cerr << "Unable to find file: " << texture_filename
				<< std::endl;
			exit(1);
		}
	}
	unsigned char* image = stbi_load(texture_filename.c_str(), &w, &h, &comp, STBI_default);
	// unsigned char* image = stbi_load("tex.jpg", &w, &h, &comp, STBI_default);
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

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {

		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			int fv = shapes[s].mesh.num_face_vertices[f];

			glBegin(GL_POLYGON);
			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
				tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
				tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
				tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
				tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
				tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
				tinyobj::real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
				tinyobj::real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];
				int current_material_id = shapes[s].mesh.material_ids[f];

				// cout << tx << ty << std::endl;				

				//glColor3f(1.0, 1.0, 1.0);
				glNormal3f(nx, ny, nz);
				glTexCoord2f(tx, ty);
				glVertex3f(vx, vy, vz);


			}
			index_offset += fv;
			glEnd();
		}
	}
	// clear texture
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image);
}

bool FileExists(const std::string& abs_filename) {
	ifstream f(abs_filename.c_str());
	return f.good();
}

string GetBaseDir(const std::string& filepath) {
	if (filepath.find_last_of("/\\") != std::string::npos)
		return filepath.substr(0, filepath.find_last_of("/\\"));
	return "";
}