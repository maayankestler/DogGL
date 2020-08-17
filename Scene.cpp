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
void keyboardupcallback(unsigned char key, int x, int y)
{
	currentInstance->keyboardUp(key, x, y);
}
void Specialcallback(int key, int x, int y)
{
	currentInstance->SpecialInput(key, x, y);
}
void Specialupcallback(int key, int x, int y)
{
	currentInstance->SpecialInputUp(key, x, y);
}
void timercallback(int v)
{
	currentInstance->timer(v);
}


Scene::Scene(int argc, char** argv) {
	// init glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE | GLUT_STENCIL);
	glutInitWindowPosition(WINDOW_POS_X, WINDOW_POS_Y);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("dog world");
	
	// create drawing objects
	this->dog = new Dog("GermanShephardLowPoly.obj", 0, 0, 0, 0.5f, glm::vec3(0, 1, 0), glm::vec3(-1, 0, 0));
	this->floor = new Floor(-10, 10, -10, 10);
	this->walls = new Walls(10, -10, 10, -10, 10);
	this->statue = new ObjectGL("venus_polygonal_statue.obj", 1, 0, -8, 0.07f);
	this->statue->angle = 160;
	this->table = new ObjectGL("Table.obj", -6, 0, 6, 0.03f);
	this->table->angle = 90;
	this->chair = new ObjectGL("Chair.obj");
	this->chair->scale = 0.75f;
	this->dog_house = new ObjectGL("niche.obj", -6, 0, -8, 1.00f);
	this->dog_house->addTask([]() { glScalef(1, 1.5f, 1); });
	this->dog_house->angle = 270;
	this->tv = new ObjectGL("televison.obj", 5.6f, 4, -8.5f, 1.0f);
	this->tv->addTask([]() { glScalef(0.7f, 1, 1); });
	this->sofa = new ObjectGL("Sofa.obj", 5.6f, 0, 2.0f, 0.075f);
	this->sofa->angle = 180;
	this->books = new ObjectGL("books.obj", 9.5f, 6, -2, 0.01f);
	this->books->angle = 270;
	this->flashlight = new Light(GL_LIGHT0, 0, 8, 0, "Flashlight.obj", 0.2f);
	this->flashlight->towardVector = glm::vec3(0, 0, 1);
	this->lamp = new Light(GL_LIGHT1, -9.15f, 4, 0, "sconce.obj", 1.0f, 180, 0, -9.15f, 0, 0);
	this->lamp->object->angle = 270;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Dear ImGui style
	ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGLUT_Init();
	ImGui_ImplGLUT_InstallFuncs(); // use the imgui glut funcs
	ImGui_ImplOpenGL2_Init();

	// configure glut funcs
	::currentInstance = this;
	glutReshapeFunc(reshapecallback);
	glutDisplayFunc(displaycallback);
	glutTimerFunc(100, timercallback, 0);
	glutKeyboardFunc(keyboardcallback);
	glutKeyboardUpFunc(keyboardupcallback);
	glutSpecialFunc(Specialcallback);
	glutSpecialUpFunc(Specialupcallback);

	glutMainLoop(); // run the main loop

	// imgui cleanup
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

	// Rendering menu
	ImGui::Render();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, aspect, 1, 100.0); // use Perspective projection
	// set dog or camera view
	if (dog_view) {
		glm::vec3 dog_view_pos = dog->getViewPos();
		glm::vec3 dog_view_targe = dog->getViewTarget();
		gluLookAt(dog_view_pos[0], dog_view_pos[1], dog_view_pos[2],
			dog_view_targe[0], dog_view_targe[1], dog_view_targe[2],
			0, 1, 0);
	}
	else {
		gluLookAt(camera_position[0], camera_position[1], camera_position[2],
			camera_target[0], camera_target[1], camera_target[2],
			0, 1, 0);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// enable opengl features
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	// add scene lights
	flashlight->addlight();
	lamp->addlight();

	// ambient intensivity
	GLfloat globalAmbientVec[4] = { ambient_intensity , ambient_intensity, ambient_intensity, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientVec);
	glLoadIdentity();

	// start drawing
	floor->draw();
	statue->draw();
	table->draw();
	dog->draw();
	dog_house->draw();
	tv->draw();
	sofa->draw();
	chair->PosX = -6; chair->PosZ = 3; chair->angle = 90; // first chair settings
	chair->draw();
	chair->PosX = -7.5f; chair->PosZ = 6.5f; chair->angle = 180; // seconde chair settings
	chair->draw();
	flashlight->draw();
	lamp->draw();
	books->draw();

	// enable blending for walls transperncy
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	walls->draw();
	glDisable(GL_BLEND);

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

	// do the key action
	key = tolower(key);
	if (key == 'w') {
		dog->walk(0.2f);
	}
	else if (key == 's') {
		dog->walk(-0.2f);
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
	else if (key == 'q') {
		dog->rotateOrgan(2.0f, DOG_HEAD, false);
	}
	else if (key == 'e') {
		dog->rotateOrgan(-2.0f, DOG_HEAD, false);
	}
	else if (key == 'g') {
		dog->rotateOrgan(1.0f, DOG_HEAD, true);
	}
	else if (key == 't') {
		dog->rotateOrgan(-1.0f, DOG_HEAD, true);
	}
	else if (key == 'v') {
		dog_view = !dog_view;
	}
	glutPostRedisplay();
}

// Handles keyboard after press
void Scene::keyboardUp(unsigned char key, int x, int y) {
	// imgui keyboard func
	ImGui_ImplGLUT_KeyboardUpFunc(key, x, y);

	// do the key action
	key = tolower(key);
	if (key == 'w' || key == 's') {
		close_legs_vert = true;
	}
	if (key == 'a' || key == 'd') {
		close_legs_hor = true;
	}
}


void Scene::SpecialInput(int key, int x, int y) {
	// imgui special func
	ImGui_ImplGLUT_SpecialFunc(key, x, y);

	// do the key action
	switch (key)
	{
	case GLUT_KEY_UP:
		dog->walk(0.2f);
		break;
	case GLUT_KEY_DOWN:
		dog->walk(-0.2f);
		break;
	case GLUT_KEY_LEFT:
		dog->rotate(5.0f);
		break;
	case GLUT_KEY_RIGHT:
		dog->rotate(-5.0f);
		break;
	}
}

void Scene::SpecialInputUp(int key, int x, int y) {
	// imgui specialup func
	ImGui_ImplGLUT_SpecialUpFunc(key, x, y);

	// make the legs move when walk or rotate
	if (key == GLUT_KEY_UP || key == GLUT_KEY_DOWN ) {
		close_legs_vert = true;
	}
	if (key == GLUT_KEY_LEFT || key == GLUT_KEY_RIGHT) {
		close_legs_hor = true;
	}
}

void Scene::timer(int v) {
	// wag tail if needed
	if (wag_tail) {
		dog->wagTail();
		glutPostRedisplay();
	}
	// move legs if needed
	if (close_legs_vert && abs(dog->organsAngles[DOG_LEFT_BACK_LEG][true]) >= 3.0f) {
		dog->moveLegs(3.0f, true);
		glutPostRedisplay();
	}
	else
	{
		close_legs_vert = false;
	}
	// move legs if needed
	if (close_legs_hor && abs(dog->organsAngles[DOG_LEFT_BACK_LEG][false]) >= 8.0f) {
		dog->moveLegs(8.0f, false);
		glutPostRedisplay();
	}
	else
	{
		close_legs_hor = false;
	}
	glutTimerFunc(1000 / 60, timercallback, v); // recursivly call to itslef for the next frame
}

// Handles the window reshape event
void Scene::reshape(GLint w, GLint h) {
	// imgui reshape func
	ImGui_ImplGLUT_ReshapeFunc(w, h);

	glViewport(0, 0, w, h);
	aspect = float(w / h);
}

void Scene::drawCoordinateArrows(void) {
	if (!debug_mode) {
		return;
	}

	// draw lines
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

	// draw letters
	glRasterPos3f(1.2f, 0.0f, 0.0f);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'x');
	glRasterPos3f(0.0f, 1.2f, 0.0f);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'y');
	glRasterPos3f(0.0f, 0.0f, 1.2f);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'z');
}

// Helper to display a little (?) mark which shows a tooltip when hovered.
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
	ImGui::Begin("Menu", &show_menu);
	ImGui::Checkbox("debug mode", &debug_mode);      HelpMarker("mark to enter debug mode");

	if (ImGui::CollapsingHeader("Room")) {
		if (debug_mode) {
			if (ImGui::TreeNode("tv")) {
				ImGui::SliderFloat("tv scale", &this->tv->scale, 0.0f, 3.0f);
				ImGui::SliderFloat("tv pos x", &this->tv->PosX, -10.0f, 10.0f);
				ImGui::SliderFloat("tv pos y", &this->tv->PosY, -10.0f, 10.0f);
				ImGui::SliderFloat("tv pos z", &this->tv->PosZ, -10.0f, 10.0f);
				ImGui::SliderFloat("tv angle", &this->tv->angle, 0.0f, 360.0f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("dog house")) {
				ImGui::SliderFloat("dog house scale", &this->dog_house->scale, 0.0f, 3.0f);
				ImGui::SliderFloat("dog house pos x", &this->dog_house->PosX, -10.0f, 10.0f);
				ImGui::SliderFloat("dog house pos y", &this->dog_house->PosY, -10.0f, 10.0f);
				ImGui::SliderFloat("dog house pos z", &this->dog_house->PosZ, -10.0f, 10.0f);
				ImGui::SliderFloat("dog house angle", &this->dog_house->angle, 0.0f, 360.0f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("sofa")) {
				ImGui::SliderFloat("sofa scale", &this->sofa->scale, 0.0f, 0.1f);
				ImGui::SliderFloat("sofa pos x", &this->sofa->PosX, -10.0f, 10.0f);
				ImGui::SliderFloat("sofa pos y", &this->sofa->PosY, -10.0f, 10.0f);
				ImGui::SliderFloat("sofa pos z", &this->sofa->PosZ, -10.0f, 10.0f);
				ImGui::SliderFloat("sofa angle", &this->sofa->angle, 0.0f, 360.0f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("statue")) {
				ImGui::SliderFloat("statue scale", &this->statue->scale, 0.0f, 0.1f);
				ImGui::SliderFloat("statue pos x", &this->statue->PosX, -10.0f, 10.0f);
				ImGui::SliderFloat("statue pos y", &this->statue->PosY, -10.0f, 10.0f);
				ImGui::SliderFloat("statue pos z", &this->statue->PosZ, -10.0f, 10.0f);
				ImGui::SliderFloat("statue angle", &this->statue->angle, 0.0f, 360.0f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("table")) {
				ImGui::SliderFloat("table scale", &this->table->scale, 0.0f, 0.1f);
				ImGui::SliderFloat("table pos x", &this->table->PosX, -10.0f, 10.0f);
				ImGui::SliderFloat("table pos y", &this->table->PosY, -10.0f, 10.0f);
				ImGui::SliderFloat("table pos z", &this->table->PosZ, -10.0f, 10.0f);
				ImGui::SliderFloat("table angle", &this->table->angle, 0.0f, 360.0f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("chair")) {
				ImGui::SliderFloat("chair scale", &this->chair->scale, 0.0f, 5.0f);
				ImGui::SliderFloat("chair pos x", &this->chair->PosX, -10.0f, 10.0f);
				ImGui::SliderFloat("chair pos y", &this->chair->PosY, -10.0f, 10.0f);
				ImGui::SliderFloat("chair pos z", &this->chair->PosZ, -10.0f, 10.0f);
				ImGui::SliderFloat("chair angle", &this->chair->angle, 0.0f, 360.0f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("books")) {
				ImGui::SliderFloat("books scale", &this->books->scale, 0.0f, 0.05f);
				ImGui::SliderFloat("books pos x", &this->books->PosX, -10.0f, 10.0f);
				ImGui::SliderFloat("books pos y", &this->books->PosY, -10.0f, 10.0f);
				ImGui::SliderFloat("books pos z", &this->books->PosZ, -10.0f, 10.0f);
				ImGui::SliderFloat("books angle", &this->books->angle, 0.0f, 360.0f);
				ImGui::TreePop();
			}
		}
		ImGui::ColorEdit3("floor color1", (float*)(&this->floor->color1)); HelpMarker("the floor's first color");
		ImGui::ColorEdit3("floor color2", (float*)(&this->floor->color2)); HelpMarker("the floor's seconde color");
		ImGui::ColorEdit3("walls color", (float*)(&this->walls->color)); HelpMarker("the walls color");
		ImGui::SliderFloat("walls alpha", &walls->alpha, 0.0f, 1.0f); HelpMarker("the walls transperncy");
		if (debug_mode) {
			ImGui::Checkbox("show south wall", &walls->showSouth);
			ImGui::Checkbox("show north wall", &walls->showNorth);
			ImGui::Checkbox("show east wall", &walls->showEast);
			ImGui::Checkbox("show west wall", &walls->showWest);
		}
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

	if (ImGui::CollapsingHeader("Dog")) {
		ImGui::Checkbox("dog view", &dog_view); HelpMarker("see the world from the dog's eyes");
		if (debug_mode) {
			ImGui::SliderFloat("dog scale", &this->dog->scale, 0.0f, 1.0f);
		}
		ImGui::SliderFloat("head angle horizontal", &this->dog->organsAngles[DOG_HEAD][false], -this->dog->maxOrgansAngles[DOG_HEAD][false], this->dog->maxOrgansAngles[DOG_HEAD][false]);
		ImGui::SliderFloat("head angle vertical", &this->dog->organsAngles[DOG_HEAD][true], -this->dog->maxOrgansAngles[DOG_HEAD][true], this->dog->maxOrgansAngles[DOG_HEAD][true]);
		ImGui::SliderFloat("tail angle horizontal", &this->dog->organsAngles[DOG_TAIL][false], -this->dog->maxOrgansAngles[DOG_TAIL][false], this->dog->maxOrgansAngles[DOG_TAIL][false]);
		ImGui::SliderFloat("tail angle vertical", &this->dog->organsAngles[DOG_TAIL][true], -this->dog->maxOrgansAngles[DOG_TAIL][true], this->dog->maxOrgansAngles[DOG_TAIL][true]);
		if (debug_mode) {
			ImGui::SliderFloat("DOG_LEFT_BACK_LEG angle horizontal", &this->dog->organsAngles[DOG_LEFT_BACK_LEG][false], -this->dog->maxOrgansAngles[DOG_LEFT_BACK_LEG][false], this->dog->maxOrgansAngles[DOG_LEFT_BACK_LEG][false]);
			ImGui::SliderFloat("DOG_LEFT_BACK_LEG angle vertical", &this->dog->organsAngles[DOG_LEFT_BACK_LEG][true], -this->dog->maxOrgansAngles[DOG_LEFT_BACK_LEG][true], this->dog->maxOrgansAngles[DOG_LEFT_BACK_LEG][true]);
			ImGui::SliderFloat("DOG_RIGHT_BACK_LEG angle horizontal", &this->dog->organsAngles[DOG_RIGHT_BACK_LEG][false], -this->dog->maxOrgansAngles[DOG_RIGHT_BACK_LEG][false], this->dog->maxOrgansAngles[DOG_RIGHT_BACK_LEG][false]);
			ImGui::SliderFloat("DOG_RIGHT_BACK_LEG angle vertical", &this->dog->organsAngles[DOG_RIGHT_BACK_LEG][true], -this->dog->maxOrgansAngles[DOG_RIGHT_BACK_LEG][true], this->dog->maxOrgansAngles[DOG_RIGHT_BACK_LEG][true]);
			ImGui::SliderFloat("DOG_LEFT_FRONT_LEG angle horizontal", &this->dog->organsAngles[DOG_LEFT_FRONT_LEG][false], -this->dog->maxOrgansAngles[DOG_LEFT_FRONT_LEG][false], this->dog->maxOrgansAngles[DOG_LEFT_FRONT_LEG][false]);
			ImGui::SliderFloat("DOG_LEFT_FRONT_LEG angle vertical", &this->dog->organsAngles[DOG_LEFT_FRONT_LEG][true], -this->dog->maxOrgansAngles[DOG_LEFT_FRONT_LEG][true], this->dog->maxOrgansAngles[DOG_LEFT_FRONT_LEG][true]);
			ImGui::SliderFloat("DOG_RIGHT_FRONT_LEG angle horizontal", &this->dog->organsAngles[DOG_RIGHT_FRONT_LEG][false], -this->dog->maxOrgansAngles[DOG_RIGHT_FRONT_LEG][false], this->dog->maxOrgansAngles[DOG_RIGHT_FRONT_LEG][false]);
			ImGui::SliderFloat("DOG_RIGHT_FRONT_LEG angle vertical", &this->dog->organsAngles[DOG_RIGHT_FRONT_LEG][true], -this->dog->maxOrgansAngles[DOG_RIGHT_FRONT_LEG][true], this->dog->maxOrgansAngles[DOG_RIGHT_FRONT_LEG][true]);
		}
		ImGui::Checkbox("wag tail", &wag_tail);      HelpMarker("the dog's tail will swing every frame");
	}

	if (ImGui::CollapsingHeader("Lights"))
	{
		ImGui::RadioButton("enable flashlight", &fe, 0); ImGui::SameLine();
		ImGui::RadioButton("disable flashlight", &fe, 1); HelpMarker("enable to add the flashlight's light to the scene");
		if (fe == 0) {
			flashlight->enable();
		}
		else
		{
			flashlight->disable();
		}
		ImGui::RadioButton("enable lamp", &le, 0); ImGui::SameLine();
		ImGui::RadioButton("disable lamp", &le, 1); HelpMarker("enable to add the lamp's light to the scene");
		if (le == 0) {
			lamp->enable();
		}
		else
		{
			lamp->disable();
		}
		ImGui::SliderFloat("adjust ambient light", &ambient_intensity, 0.0f, 1.0f);                 HelpMarker("control the intensity of the global ambient");
		ImGui::ColorEdit3("lamp light color", (float*)(&this->lamp->color));						HelpMarker("chose the color of the lamp's light");
		ImGui::ColorEdit3("flashlight light color", (float*)(&this->flashlight->color));            HelpMarker("chose the color of the flashlight's light");
		ImGui::SliderFloat("flashlight position x", &this->flashlight->position[0], -10.0f, 10.0f); HelpMarker("the x coordinate of the flashlight position");
		ImGui::SliderFloat("flashlight position y", &this->flashlight->position[1], -10.0f, 10.0f); HelpMarker("the y coordinate of the flashlight position");
		ImGui::SliderFloat("flashlight position z", &this->flashlight->position[2], -10.0f, 10.0f); HelpMarker("the x coordinate of the flashlight position");
		ImGui::SliderFloat("flashlight target x", &this->flashlight->target[0], -10.0f, 10.0f);     HelpMarker("the x coordinate of the position that the flashlight will look at");
		ImGui::SliderFloat("flashlight target y", &this->flashlight->target[1], -10.0f, 10.0f);     HelpMarker("the y coordinate of the position that the flashlight will look at");
		ImGui::SliderFloat("flashlight target z", &this->flashlight->target[2], -10.0f, 10.0f);     HelpMarker("the z coordinate of the position that the flashlight will look at");
		ImGui::SliderFloat("flashlight cutoff", &this->flashlight->cutoff, 0.0f, 180.0f);           HelpMarker("the angle that the flashlight's light is affective");
		ImGui::SliderFloat("flashlight exponent", &this->flashlight->exponent, 0.0f, 30.0f);        HelpMarker("the intensity distribution of the flashlight's light");

		if (debug_mode) {
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
							this->flashlight->object->scale = 0.2f;
							break;
						case 1:
							this->flashlight->object = new ObjectGL("Linterna.obj");
							this->flashlight->towardVector = glm::vec3(-1, 0, 0);
							this->flashlight->object->scale = 0.2f;
							break;
						case 2:
							this->flashlight->object = new ObjectGL("3d-model.obj");
							this->flashlight->towardVector = glm::vec3(-1, 0, 0);
							this->flashlight->object->scale = 0.01f;
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
			HelpMarker("chose the flashlight drawing object");

			ImGui::SliderFloat("lamp scale", &this->lamp->object->scale, 0.0f, 3.0f);
			ImGui::SliderFloat("lamp angle", &this->lamp->object->angle, 0.0f, 360.0f);
			ImGui::SliderFloat("lamp position x", &this->lamp->position[0], -10.0f, 10.0f); HelpMarker("the x coordinate of the lamp position");
			ImGui::SliderFloat("lamp position y", &this->lamp->position[1], -10.0f, 10.0f); HelpMarker("the y coordinate of the lamp position");
			ImGui::SliderFloat("lamp position z", &this->lamp->position[2], -10.0f, 10.0f); HelpMarker("the x coordinate of the lamp position");
			ImGui::SliderFloat("lamp target x", &this->lamp->target[0], -10.0f, 10.0f);     HelpMarker("the x coordinate of the position that the lamp will look at");
			ImGui::SliderFloat("lamp target y", &this->lamp->target[1], -10.0f, 10.0f);     HelpMarker("the y coordinate of the position that the lamp will look at");
			ImGui::SliderFloat("lamp target z", &this->lamp->target[2], -10.0f, 10.0f);     HelpMarker("the z coordinate of the position that the lamp will look at");
			ImGui::SliderFloat("lamp cutoff", &this->lamp->cutoff, 0.0f, 180.0f);           HelpMarker("the angle that the lamp's light is affective");
			ImGui::SliderFloat("lamp exponent", &this->lamp->exponent, 0.0f, 30.0f);        HelpMarker("the intensity distribution of the lamp's light");
		}
	}

	if (ImGui::CollapsingHeader("help")) {
		ImGui::Text("Welcome to my opengl dog's room project");
		ImGui::TextWrapped("you can go in the room and explore the objects, you can also use this menu to affect the room.");
		ImGui::TextWrapped("normally you can see some basic options and with debug mode you almost full control on the scene");
		ImGui::Separator();
		ImGui::Text("ROOM:");
		ImGui::TextWrapped("this category affect the room and it's object, for example you can change the walls and the floor color");
		ImGui::Separator();
		ImGui::Text("CAMERA:");
		ImGui::TextWrapped("this category affect the camera, you can change the settings to see the room from diffrent angles");
		ImGui::Separator();
		ImGui::Text("DOG:");
		ImGui::TextWrapped("this category affect our hero, the dog, you can move it's head and tail.");
		ImGui::TextWrapped("the main feature here is to se the world from the dog view");
		ImGui::Separator();
		ImGui::Text("LIGHTS:");
		ImGui::TextWrapped("this category affect the lights' the flashlight and the lamp.");
		ImGui::TextWrapped("you can control the light settings and see how it's affect the objects");
		ImGui::Separator();
		ImGui::Text("the available keyboards:");
		ImGui::Columns(2, "keyboard");
		ImGui::Separator();
		ImGui::Text("Key"); ImGui::NextColumn();
		ImGui::Text("Action"); ImGui::NextColumn();
		ImGui::Separator();
		ImGui::Text("'w' / up arrow"); ImGui::NextColumn(); ImGui::Text("move the dog forward"); ImGui::NextColumn();
		ImGui::Text("'s' / down arrow"); ImGui::NextColumn(); ImGui::Text("move the dog backward"); ImGui::NextColumn();
		ImGui::Text("'d' / right arrow"); ImGui::NextColumn(); ImGui::Text("turn the dog right"); ImGui::NextColumn();
		ImGui::Text("'a' / left arrow"); ImGui::NextColumn(); ImGui::Text("turn the dog left"); ImGui::NextColumn();
		ImGui::Text("'q'"); ImGui::NextColumn(); ImGui::Text("turn the dog head left"); ImGui::NextColumn();
		ImGui::Text("'e'"); ImGui::NextColumn(); ImGui::Text("turn the dog head right"); ImGui::NextColumn();
		ImGui::Text("'g'"); ImGui::NextColumn(); ImGui::Text("turn the dog head down"); ImGui::NextColumn();
		ImGui::Text("'t'"); ImGui::NextColumn(); ImGui::Text("turn the dog head up"); ImGui::NextColumn();
		ImGui::Text("'m'"); ImGui::NextColumn(); ImGui::Text("show or hide menu"); ImGui::NextColumn();
		ImGui::Text("'v'"); ImGui::NextColumn(); ImGui::Text("change view (between camera and dog)"); ImGui::NextColumn();
		ImGui::Columns(1);
	}

	if (debug_mode) {
		ImGui::Separator();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

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