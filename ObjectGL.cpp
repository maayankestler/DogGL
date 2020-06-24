#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "ObjectGL.h"

ObjectGL::ObjectGL(string inputfile) {
	this->inputfile = inputfile;

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, this->inputfile.c_str());

	if (!warn.empty()) {
		std::cout << warn << std::endl;
	}

	if (!err.empty()) {
		std::cerr << err << std::endl;
	}

	if (!ret) {
		exit(1);
	}

	this->attrib = attrib;
	this->shapes = shapes;
	this->materials = materials;

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


void ObjectGL::draw() {
	// bind Texture
	glBindTexture(GL_TEXTURE_2D, this->texture_id);

	// Loop over shapes
	for (size_t s = 0; s < this->shapes.size(); s++) {

		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < this->shapes[s].mesh.num_face_vertices.size(); f++) {
			int fv = this->shapes[s].mesh.num_face_vertices[f];

			glBegin(GL_POLYGON);
			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				// access to vertex
				tinyobj::index_t idx = this->shapes[s].mesh.indices[index_offset + v];
				tinyobj::real_t vx = this->attrib.vertices[3 * idx.vertex_index + 0];
				tinyobj::real_t vy = this->attrib.vertices[3 * idx.vertex_index + 1];
				tinyobj::real_t vz = this->attrib.vertices[3 * idx.vertex_index + 2];
				tinyobj::real_t nx = this->attrib.normals[3 * idx.normal_index + 0];
				tinyobj::real_t ny = this->attrib.normals[3 * idx.normal_index + 1];
				tinyobj::real_t nz = this->attrib.normals[3 * idx.normal_index + 2];
				tinyobj::real_t tx = this->attrib.texcoords[2 * idx.texcoord_index + 0];
				tinyobj::real_t ty = this->attrib.texcoords[2 * idx.texcoord_index + 1];
				// int current_material_id = this->shapes[s].mesh.material_ids[f];			

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