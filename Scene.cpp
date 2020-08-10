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
void Specialcallback(int key, int x, int y)
{
	currentInstance->SpecialInput(key, x, y);
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
	this->dog = new ObjectGL("GermanShephardLowPoly.obj", 0, 0, 0);
	this->dog->towardVector = glm::vec3(-1, 0, 0);
	this->dog->addTask([]() { glScalef(0.5f, 0.5f, 0.5f); });
	/*this->dog->addTask([]() { glScalef(2.0f, 2.0f, 2.0f); }, "head_Plane.002");
	this->dog->addTask([]() { glRotatef(90, 0, 1, 0); }, "head_Plane.002");*/
	this->floor = new Floor(-10, 10, -10, 10);
	this->statue = new ObjectGL("venus_polygonal_statue.obj", -8, 0, -8);
	this->statue->addTask([]() { glScalef(0.07f, 0.07f, 0.07f); });
	this->statue->angle = 180;
	this->table = new ObjectGL("abciuppa_table_w_m_01.obj", 6, 0, 6);
	this->table->addTask([]() { glScalef(3.0f, 3.0f, 3.0f); });
	this->flashlight = new Light(GL_LIGHT0, 0, 8, 0, "Flashlight.obj");
	this->flashlight->towardVector = glm::vec3(0, 0, 1);
	this->flashlight->object->addTask([]() { glScalef(0.2f, 0.2f, 0.2f); });

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
	glutSpecialFunc(Specialcallback);

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

	glShadeModel(GL_SMOOTH);
	//glEnable(GL_BLEND);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	flashlight->addlight();
	

	GLfloat globalAmbientVec[4] = { ambient_intensity , ambient_intensity, ambient_intensity, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientVec);
	glLoadIdentity();

	// start drawing
	floor->draw();
	statue->draw();
	table->draw();
	dog->draw();
	flashlight->draw();

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

void Scene::SpecialInput(int key, int x, int y)
{
	ImGui_ImplGLUT_SpecialFunc(key, x, y);

	switch (key)
	{
	case GLUT_KEY_UP:
		dog->walk(1.0f);
		break;
	case GLUT_KEY_DOWN:
		dog->walk(-1.0f);
		break;
	case GLUT_KEY_LEFT:
		dog->rotate(5.0f);
		break;
	case GLUT_KEY_RIGHT:
		dog->rotate(-5.0f);
		break;
	}
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
				ImGui::ColorEdit3("light color", (float*)(&this->flashlight->color));                  HelpMarker("chose the color of the light");
				ImGui::SliderFloat("light position x", &this->flashlight->position[0], -10.0f, 10.0f); HelpMarker("the x coordinate of the light position");
				ImGui::SliderFloat("light position y", &this->flashlight->position[1], -10.0f, 10.0f); HelpMarker("the y coordinate of the light position");
				ImGui::SliderFloat("light position z", &this->flashlight->position[2], -10.0f, 10.0f); HelpMarker("the x coordinate of the light position");
				ImGui::SliderFloat("light target x", &this->flashlight->target[0], -10.0f, 10.0f);     HelpMarker("the x coordinate of the position that the light will look at");
				ImGui::SliderFloat("light target y", &this->flashlight->target[1], -10.0f, 10.0f);     HelpMarker("the y coordinate of the position that the light will look at");
				ImGui::SliderFloat("light target z", &this->flashlight->target[2], -10.0f, 10.0f);     HelpMarker("the z coordinate of the position that the light will look at");
				ImGui::SliderFloat("light cutoff", &this->flashlight->cutoff, 0.0f, 180.0f);           HelpMarker("the angle that the light is affective");
				ImGui::SliderFloat("light exponent", &this->flashlight->exponent, 0.0f, 30.0f);        HelpMarker("the intensity distribution of the light");

				const char* items[] = { "Flashlight.obj", "Linterna.obj", "3d-model.obj" };
				static int item_current_idx = 0;
				if (ImGui::BeginCombo("flash light", items[item_current_idx]))
				{
					for (int n = 0; n < IM_ARRAYSIZE(items); n++)
					{
						const bool is_selected = (item_current_idx == n);
						if (ImGui::Selectable(items[n], is_selected)) {
							item_current_idx = n;
							switch (item_current_idx)
							{
							case 0:
								this->flashlight->object = new ObjectGL("Flashlight.obj");
								this->flashlight->towardVector = glm::vec3(0, 0, 1);
								this->flashlight->object->addTask([]() { glScalef(0.2f, 0.2f, 0.2f); });
								break;
							case 1:
								this->flashlight->object = new ObjectGL("Linterna.obj");
								this->flashlight->towardVector = glm::vec3(-1, 0, 0);
								this->flashlight->object->addTask([]() { glScalef(0.2f, 0.2f, 0.2f); });
								break;
							case 2:
								this->flashlight->object = new ObjectGL("3d-model.obj");
								this->flashlight->towardVector = glm::vec3(-1, 0, 0);
								this->flashlight->object->addTask([]() { glScalef(0.01f, 0.01f, 0.01f); });
								break;
							}
						}

						// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
						if (is_selected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				HelpMarker("chose the flashlight form");
			}

			if (ImGui::CollapsingHeader("help")) {
				ImGui::Columns(2, "keyboard");
				ImGui::Separator();
				ImGui::Text("Key"); ImGui::NextColumn();
				ImGui::Text("Action"); ImGui::NextColumn();
				ImGui::Separator();
				ImGui::Text("'w' / up arrow"); ImGui::NextColumn(); ImGui::Text("move the dog forward"); ImGui::NextColumn();
				ImGui::Text("'s' / down arrow"); ImGui::NextColumn(); ImGui::Text("move the dog backward"); ImGui::NextColumn();
				ImGui::Text("'d' / right arrow"); ImGui::NextColumn(); ImGui::Text("turn the dog right"); ImGui::NextColumn();
				ImGui::Text("'a' / left arrow"); ImGui::NextColumn(); ImGui::Text("turn the dog left"); ImGui::NextColumn();
				ImGui::Text("'m'"); ImGui::NextColumn(); ImGui::Text("show or hide menu"); ImGui::NextColumn();
				ImGui::Columns(1);
			}

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.5f);
			ImGui::Indent(ImGui::GetWindowWidth() * 0.25f);
			if (ImGui::Button("EXIT", ImVec2(ImGui::GetWindowSize().x * 0.5f, 0.0f))) {
				ImGui::OpenPopup("Exit?");
			}
			ImGui::PopStyleColor(3);

			// Always center this window when appearing
			ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			if (ImGui::BeginPopupModal("Exit?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("do you sure you want to exit?\n\n");
				ImGui::Separator();

				if (ImGui::Button("yes", ImVec2(120, 0))) {
					glutLeaveMainLoop();
				}
				ImGui::SetItemDefaultFocus();
				ImGui::SameLine();
				if (ImGui::Button("no", ImVec2(120, 0))) {
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		ImGui::End();
	}
}