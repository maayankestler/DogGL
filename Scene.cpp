#include "Scene.h"

// creating callbacks to the class functions as describe in 
// https://stackoverflow.com/questions/3589422/using-opengl-glutdisplayfunc-within-class
Scene* currentInstance;
void displaycallback()
{
	currentInstance->display();
}
void reshapecallback(GLint w, GLint h)
{
	currentInstance->reshape(w, h);
}
void keyboardcallback(unsigned char key, int x, int y)
{
	currentInstance->keyboard(key, x, y);
}

// Initializes GLUT, the display mode, and main window; registers callbacks;
// enters the main event loop.
Scene::Scene(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE | GLUT_STENCIL);
	glutInitWindowPosition(WINDOW_POS_X, WINDOW_POS_Y);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("dog world");
	
	// create drawing objects
	this->dog = new ObjectGL("GermanShephardLowPoly2.obj", 0, 0, 0);
	this->dog->towardVector = glm::vec3(-1, 0, 0);
	this->dog->addTask([]() { glScalef(0.5f, 0.5f, 0.5f); });
	this->floor = new Floor(-10, 10, -10, 10);
	this->statue = new ObjectGL("venus_polygonal_statue.obj", -8, 0, -8);
	this->statue->addTask([]() { glScalef(0.07f, 0.07f, 0.07f); });
	this->statue->angle = 180;
	this->table = new ObjectGL("abciuppa_table_w_m_01.obj", 6, 0, 6);
	this->table->addTask([]() { glScalef(3.0f, 3.0f, 3.0f); });
	this->lamp = new Light(GL_LIGHT0, 0, 10, 0, "Flashlight.obj");
	this->lamp->towardVector = glm::vec3(0, 0, 1);
	this->lamp->lamp->addTask([]() { glScalef(0.2f, 0.2f, 0.2f); });

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Dear ImGui style
	// ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGLUT_Init();
	ImGui_ImplGLUT_InstallFuncs(); // use the imgui glut funcs
	ImGui_ImplOpenGL2_Init();

	// configure glut funcs
	::currentInstance = this;
	glutReshapeFunc(reshapecallback);
	glutDisplayFunc(displaycallback);
	//glutTimerFunc(100, timercallback, 0);
	glutKeyboardFunc(keyboardcallback);

	glutMainLoop();

	// Cleanup
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplGLUT_Shutdown();
	ImGui::DestroyContext();
}

// Handles the display callback to show what we have drawn.
void Scene::display() {
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplGLUT_NewFrame();

	// display the menu with imgui
	if (show_menu)
        display_menu();

	// Rendering
	ImGui::Render();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, aspect, 1, 100.0);
	gluLookAt(camera_position[0], camera_position[1], camera_position[2],
              camera_target[0], camera_target[1], camera_target[2],
		      0, 1, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_DEPTH_TEST);

	// lighting
	glShadeModel(GL_SMOOTH);
	//glEnable(GL_BLEND);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	lamp->addlight();
	

	GLfloat globalAmbientVec[4] = { ambient_intensity , ambient_intensity, ambient_intensity, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientVec);
	glLoadIdentity();

	// start drawing
	floor->draw();
	statue->draw();
	table->draw();
	dog->draw();
	lamp->draw();

	// add Coordinate Arrows for debug
	drawCoordinateArrows();
	//imgui does not handle light well
	glDisable(GL_LIGHTING);
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	glEnable(GL_LIGHTING);

	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
}

// Handles keyboard press
void Scene::keyboard(unsigned char key, int x, int y) {
	// imgui keyboard func
	ImGui_ImplGLUT_KeyboardFunc(key, x, y);

	key = tolower(key);
	if (key == 'w') {
		dog->walk(1.0f);
	}
	else if (key == 's') {
		dog->walk(-1.0f);
	}
	else if (key == 'd') {
		dog->rotate(-5.0f);
	}
	else if (key == 'a') {
		dog->rotate(5.0f);
	}
	else if (key == 'm') {
		show_menu = !show_menu;
	}
	glutPostRedisplay();
}


// Handles the window reshape event
void Scene::reshape(GLint w, GLint h) {
	// imgui reshape func
	ImGui_ImplGLUT_ReshapeFunc(w, h);

	glViewport(0, 0, w, h);
	aspect = float(w / h);
}

void Scene::drawCoordinateArrows(void) {
	if (!show_coordinates) {
		return;
	}

	glColor3f(1.0, 0.0, 0.0);

	glBegin(GL_LINE_STRIP);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(1.0, 0.0, 0.0);
	glEnd();


	glBegin(GL_LINE_STRIP);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 1.0, 0.0);
	glEnd();


	glBegin(GL_LINE_STRIP);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 1.0);
	glEnd();

	glRasterPos3f(1.2f, 0.0f, 0.0f);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'x');
	glRasterPos3f(0.0f, 1.2f, 0.0f);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'y');
	glRasterPos3f(0.0f, 0.0f, 1.2f);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'z');
}

// Helper to display a little(? ) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc)
{
	ImGui::SameLine();
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void Scene::display_menu()
{
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		ImGui::Begin("Menu", &show_menu);
			ImGui::Text("Welcome to my dog room project");  // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state

			//static int selected_floor = -1;
			//const char* floors[] = { "floor.jpg", "floor2.jpg","floor3.jpg", "floor4.jpg" };

			//// floor texture selection popup
			//if (ImGui::Button("Select.."))
			//	ImGui::OpenPopup("my_select_popup");
			//ImGui::SameLine();
			//ImGui::TextUnformatted(selected_floor == -1 ? "<None>" : floors[selected_floor]);
			//if (ImGui::BeginPopup("my_select_popup"))
			//{
			//	ImGui::Text("floor texture");
			//	ImGui::Separator();
			//	for (int i = 0; i < IM_ARRAYSIZE(floors); i++) {
			//		if (ImGui::Selectable(floors[i])) {
			//			selected_floor = i;
			//			this->floor = new Floor(floors[selected_floor], -10, 10, -10, 10);
			//		}
			//	}
			//	ImGui::EndPopup();
			//}

			if (ImGui::CollapsingHeader("Room")) {
				ImGui::Checkbox("show coordinate arrows", &show_coordinates);      HelpMarker("mark to see the coordinate arrows in the scence");
				ImGui::ColorEdit3("floor color1", (float*)(&this->floor->color1)); HelpMarker("the floor's first color");
				ImGui::ColorEdit3("floor color2", (float*)(&this->floor->color2)); HelpMarker("the floor's seconde color");
			}

			if (ImGui::CollapsingHeader("Camera"))
			{
				ImGui::SliderFloat("camera position x", &camera_position[0], -20.0f, 20.0f); HelpMarker("the x coordinate of the camera position");
				ImGui::SliderFloat("camera position y", &camera_position[1], -5.0f, 20.0f);  HelpMarker("the y coordinate of the camera position");
				ImGui::SliderFloat("camera position z", &camera_position[2], -30.0f, 30.0f); HelpMarker("the z coordinate of the camera position");
				ImGui::SliderFloat("camera target x", &camera_target[0], -20.0f, 20.0f);     HelpMarker("the x coordinate of the position that the camera will look at");
				ImGui::SliderFloat("camera target y", &camera_target[1], -15.0f, 15.0f);     HelpMarker("the y coordinate of the position that the camera will look at");
				ImGui::SliderFloat("camera target z", &camera_target[2], -20.0f, 20.0f);     HelpMarker("the z coordinate of the position that the camera will look at");
			}
			
			if (ImGui::CollapsingHeader("Lights"))
			{
				ImGui::SliderFloat("ambient intensity", &ambient_intensity, 0.0f, 1.0f);         HelpMarker("control the intensity of the global ambient");
				ImGui::ColorEdit3("light color", (float*)(&this->lamp->color));                  HelpMarker("chose the color of the light");
				ImGui::SliderFloat("light position x", &this->lamp->position[0], -10.0f, 10.0f); HelpMarker("the x coordinate of the light position");
				ImGui::SliderFloat("light position y", &this->lamp->position[1], -10.0f, 10.0f); HelpMarker("the y coordinate of the light position");
				ImGui::SliderFloat("light position z", &this->lamp->position[2], -10.0f, 10.0f); HelpMarker("the x coordinate of the light position");
				ImGui::SliderFloat("light target x", &this->lamp->target[0], -10.0f, 10.0f);     HelpMarker("the x coordinate of the position that the light will look at");
				ImGui::SliderFloat("light target y", &this->lamp->target[1], -10.0f, 10.0f);     HelpMarker("the y coordinate of the position that the light will look at");
				ImGui::SliderFloat("light target z", &this->lamp->target[2], -10.0f, 10.0f);     HelpMarker("the z coordinate of the position that the light will look at");
				ImGui::SliderFloat("light cutoff", &this->lamp->cutoff, 0.0f, 180.0f);           HelpMarker("the angle that the light is affective");
				ImGui::SliderFloat("light exponent", &this->lamp->exponent, 0.0f, 30.0f);        HelpMarker("the intensity distribution of the light");

				int flash_idx = -1;
				if (ImGui::Combo("flash light texture", &flash_idx, "Flashlight1\0Flashlight2\0Flashlight3\0Flashlight4\0"))
				{
					switch (flash_idx)
					{
						case 0:
							this->lamp = new Light(GL_LIGHT0, this->lamp->position[0], this->lamp->position[1], this->lamp->position[2], "Flashlight.obj",
												   this->lamp->cutoff, this->lamp->exponent,
												   this->lamp->target[0], this->lamp->target[0], this->lamp->target[0]);
							this->lamp->towardVector = glm::vec3(0, 0, 1);
							this->lamp->lamp->addTask([]() { glScalef(0.2f, 0.2f, 0.2f); });
							break;
						case 1:
							this->lamp = new Light(GL_LIGHT0, this->lamp->position[0], this->lamp->position[1], this->lamp->position[2], "Linterna.obj",
												   this->lamp->cutoff, this->lamp->exponent,
								                   this->lamp->target[0], this->lamp->target[0], this->lamp->target[0]);
							this->lamp->towardVector = glm::vec3(-1, 0, 0);
							this->lamp->lamp->addTask([]() { glScalef(0.2f, 0.2f, 0.2f); });
							break;
						case 2:
							this->lamp = new Light(GL_LIGHT0, this->lamp->position[0], this->lamp->position[1], this->lamp->position[2], "",
								this->lamp->cutoff, this->lamp->exponent,
								this->lamp->target[0], this->lamp->target[0], this->lamp->target[0]);
							this->lamp->towardVector = glm::vec3(0, 0, -1); 
							break;
						case 3:
							this->lamp = new Light(GL_LIGHT0, this->lamp->position[0], this->lamp->position[1], this->lamp->position[2], "3d-model.obj",
								                   this->lamp->cutoff, this->lamp->exponent,
								                   this->lamp->target[0], this->lamp->target[0], this->lamp->target[0]);
							this->lamp->towardVector = glm::vec3(-1, 0, 0);
							this->lamp->lamp->addTask([]() { glScalef(0.01f, 0.01f, 0.01f); });
							break;
					}
				}
			}

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			// ImGui::PushID(0);
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));
			if (ImGui::Button("EXIT")) {
				show_exit_window = true;
			}
			ImGui::PopStyleColor(3);
			// ImGui::PopID();

			if (show_exit_window)
			{
				ImGui::Begin("Exit Window", &show_exit_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
					ImGui::Text("do you sure you want to exit?");
					if (ImGui::Button("yes")) {
						glutDestroyWindow(1);
					}
					ImGui::SameLine();
					if (ImGui::Button("no")) {
						show_exit_window = false;
					}
				ImGui::End();
			}

		ImGui::End();
	}
}