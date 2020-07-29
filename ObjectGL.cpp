#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "ObjectGL.h"

ObjectGL::ObjectGL(string inputfile, GLfloat PosX, GLfloat PosY, GLfloat PosZ) {
	this->inputfile = inputfile;
	this->upVector = glm::vec3(0, 0, 0);
	this->towardVector = glm::vec3(0, 0, 0);

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

	// create textures
	this->textures[""] = 0;
	GLuint texture_id;
	int w, h;
	int comp;
	string texture_filename;
	for (size_t m = 0; m < materials.size(); m++) {
		tinyobj::material_t* mp = &materials[m];
		texture_filename = mp->diffuse_texname;
		if (this->textures.find(texture_filename) == this->textures.end()) {
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
				assert(0);
			}
			glBindTexture(GL_TEXTURE_2D, 0);
			stbi_image_free(image);
			this->textures.insert(make_pair(mp->diffuse_texname, texture_id));
		}
	}

	setPosition(PosX, PosY, PosZ);
}

void ObjectGL::draw() {
	glPushMatrix();

	// call all the tasks in the vector
	for (function<void()> task : this->tasks) {
		task();
	}

	glTranslatef(PosX, PosY, PosZ);
	glRotatef(angle, this->upVector.x, this->upVector.y, this->upVector.z);

	// Loop over shapes
	for (size_t s = 0; s < this->shapes.size(); s++) {

		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < this->shapes[s].mesh.num_face_vertices.size(); f++) {
			// bind Texture
			int current_material_id = this->shapes[s].mesh.material_ids[f];
			string diffuse_texname = this->materials[current_material_id].diffuse_texname;
			glBindTexture(GL_TEXTURE_2D, this->textures[diffuse_texname]);

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
				if (idx.texcoord_index != -1) {
					tinyobj::real_t tx = this->attrib.texcoords[2 * idx.texcoord_index + 0];
					tinyobj::real_t ty = this->attrib.texcoords[2 * idx.texcoord_index + 1];
					glTexCoord2f(tx, ty);
				}

				glNormal3f(nx, ny, nz);
				glVertex3f(vx, vy, vz);

				// Combine normal and diffuse to get color.
				float diffuse_factor = 0.8; // TODO get diffuse_factor as var from imgui
				float normal_factor = 1 - diffuse_factor;
				float c[3] = { nx * normal_factor + materials[current_material_id].diffuse[0] * diffuse_factor,
							   ny * normal_factor + materials[current_material_id].diffuse[1] * diffuse_factor,
							   nz * normal_factor + materials[current_material_id].diffuse[2] * diffuse_factor };
				float len2 = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];
				if (len2 > 0.0f) {
					float len = sqrtf(len2);

					c[0] /= len;
					c[1] /= len;
					c[2] /= len;
				}
				glColor3f(c[0] * 0.5 + 0.5, c[1] * 0.5 + 0.5, c[2] * 0.5 + 0.5);
			}
			index_offset += fv;
			glEnd();
		}
	}
	// clear texture
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
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

//Set the dog's position (center of torso position)
void ObjectGL::setPosition(GLfloat x, GLfloat y, GLfloat z) {
	this->PosX = x;
	this->PosY = y;
	this->PosZ = z;
	// addTask([x, y, z]() { glTranslatef(x, y, z); });
}

void ObjectGL::walk(float distance) {
	float x = this->PosX + distance * this->towardVector.x;
	float y = this->PosY + distance * this->towardVector.y;
	float z = this->PosZ + distance * this->towardVector.z;
	setPosition(x, y, z);
}

void ObjectGL::addTask(function<void()> func) {
	this->tasks.push_back(func);
}

void ObjectGL::rotate(GLfloat angle) {
	float rad_angle = (angle / 180) * PI; // use radians
	glm::mat4 rotationMat(1);
	glm::vec3 cross = glm::cross(this->upVector, this->towardVector);
	rotationMat = glm::rotate(rotationMat, rad_angle, this->upVector);
	this->towardVector = glm::vec3(rotationMat * glm::vec4(this->towardVector, 1.0));
	this->angle += angle;
	//float x = this->upVector.x;
	//float y = this->upVector.y;
	//float z = this->upVector.z;
	//addTask([angle, x, y, z]() { glRotatef(angle, x, y, z); });
}