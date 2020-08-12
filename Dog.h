#pragma once
using namespace std;
#include "ObjectGL.h"

// organs macro
#define DOG_HEAD			0
#define DOG_TAIL			1
#define DOG_LEFT_BACK_LEG   2
#define DOG_RIGHT_BACK_LEG  3
#define DOG_LEFT_FRONT_LEG  4
#define DOG_RIGHT_FRONT_LEG 5

class Dog : public ObjectGL {
	//using ObjectGL::ObjectGL;
	public:
		Dog(string inputfile, GLfloat PosX = 0, GLfloat PosY = 0, GLfloat PosZ = 0,
			glm::vec3 upVector = glm::vec3(0, 1, 0), glm::vec3 towardVector = glm::vec3(0, 0, 0), GLfloat angle = 0);
		map<int, vector<GLfloat>> organsPos = {
		//      organ			                 pos
			{DOG_HEAD,            { -2.4644, 4.9088, 0        } },
			{DOG_TAIL,            { 2.9906,  3.8998, 0        } },
			{DOG_LEFT_BACK_LEG,   { 1.8531,  3.6308, 1.0157   } },
			{DOG_RIGHT_BACK_LEG,  { 1.8531,  3.6308, -1.0157  } },
			{DOG_LEFT_FRONT_LEG,  { -1.185,  2.2522, 0.78169  } },
			{DOG_RIGHT_FRONT_LEG, { -1.185,  2.2522, -0.78169 } }
		};
		map<int, map<bool, float>> maxOrgansAngles = {
	    //    organ			         vertical max   horizontal max
			{DOG_HEAD,            { {true, 30.0f}, {false, 60.0f} } },
			{DOG_TAIL,            { {true, 30.0f}, {false, 60.0f} } },
			{DOG_LEFT_BACK_LEG,   { {true, 30.0f}, {false, 30.0f} } },
			{DOG_RIGHT_BACK_LEG,  { {true, 30.0f}, {false, 30.0f} } },
			{DOG_LEFT_FRONT_LEG,  { {true, 30.0f}, {false, 30.0f} } },
			{DOG_RIGHT_FRONT_LEG, { {true, 30.0f}, {false, 30.0f} } }
		};
		map<int, map<bool, float>> organsAngles = {
		//    organ			        vertical max   horizontal max
			{DOG_HEAD,            { {true, 0.0f}, {false, 0.0f} } },
			{DOG_TAIL,            { {true, 0.0f}, {false, 0.0f} } },
			{DOG_LEFT_BACK_LEG,   { {true, 0.0f}, {false, 0.0f} } },
			{DOG_RIGHT_BACK_LEG,  { {true, 0.0f}, {false, 0.0f} } },
			{DOG_LEFT_FRONT_LEG,  { {true, 0.0f}, {false, 0.0f} } },
			{DOG_RIGHT_FRONT_LEG, { {true, 0.0f}, {false, 0.0f} } }
		};
		void rotateOrgan(GLfloat angle, int organ, bool vertical);
		void walk(GLfloat distance);
	private:
		glm::vec3 sideVector;
		void handleRotation(int organ);
};