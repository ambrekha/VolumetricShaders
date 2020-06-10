#include <fstream>
#include <string>
#include <vector>

#include <GL/glew.h>

#include <SDL2/SDL_main.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#undef main

#include "NFD/nfd.h"

#include "helperFunctions.h"

#include "myShader.h"
#include "myCamera.h"
#include "myObject3D.h"

#include <glm/glm.hpp>

using namespace std;

#define INITIAL_WINDOW_WIDTH 640
#define INITIAL_WINDOW_HEIGHT 480

// SDL variables
SDL_Window* window;
SDL_GLContext glContext;
 
int mouse_position[2];
bool mouse_button_pressed = false;
bool quit = false;
bool windowsize_changed = true;
bool crystalballorfirstperson_view = true;

// Camera parameters.
myCamera *cam1; 

// Mesh object
myObject3D *obj1;

//Shader
myShader *shader1;

//Point to draw to illustrate picking
glm::vec3 picked_point;

// Process the event.  
void processEvents(SDL_Event current_event)
{
	switch (current_event.type)
	{
		// window close button is pressed
		case SDL_QUIT:
		{
			quit = true;
			break;
		}
		case SDL_KEYDOWN:
		{
			if (current_event.key.keysym.sym == SDLK_ESCAPE)
				quit = true;
			if (current_event.key.keysym.sym == SDLK_UP)
				cam1->moveForward(0.1f);
			if (current_event.key.keysym.sym == SDLK_DOWN)
				cam1->moveBack(0.1f);
			if (current_event.key.keysym.sym == SDLK_LEFT)
				cam1->turnLeft(0.1f);
			if (current_event.key.keysym.sym == SDLK_RIGHT)
				cam1->turnRight(0.1f);
			if (current_event.key.keysym.sym == SDLK_v)
				crystalballorfirstperson_view = !crystalballorfirstperson_view;
			else if (current_event.key.keysym.sym == SDLK_o)
			{
				nfdchar_t *outPath = NULL;
				nfdresult_t result = NFD_OpenDialog("obj", NULL, &outPath);
				if (result != NFD_OKAY) return;
				myObject3D *obj_tmp = new myObject3D();
				if (!obj_tmp->readMesh(outPath))
				{
					delete obj_tmp;
					return;
				}
				delete obj1;
				obj1 = obj_tmp;
				obj1->computeNormals();
			}
			break;
		}
		case SDL_MOUSEBUTTONDOWN:
		{
			mouse_position[0] = current_event.button.x;
			mouse_position[1] = current_event.button.y;
			mouse_button_pressed = true;

			const Uint8 *state = SDL_GetKeyboardState(NULL);
			if (state[SDL_SCANCODE_LCTRL])
			{
				glm::vec3 ray = cam1->constructRay(mouse_position[0], mouse_position[1]);
				picked_point = obj1->closestVertex(ray, cam1->camera_eye);
			}
			break;
		}
		case SDL_MOUSEBUTTONUP:
		{
			mouse_button_pressed = false;
			break;
		}
		case SDL_MOUSEMOTION:
		{
			int x = current_event.motion.x;
			int y = current_event.motion.y;

			int dx = x - mouse_position[0];
			int dy = y - mouse_position[1];

			mouse_position[0] = x;
			mouse_position[1] = y;

			if ((dx == 0 && dy == 0) || !mouse_button_pressed) return;

			if ( (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) && crystalballorfirstperson_view)
				cam1->crystalball_rotateView(dx, dy);
			else if ((SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) && !crystalballorfirstperson_view)
				cam1->firstperson_rotateView(dx, dy);
			else if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT))
				cam1->panView(dx, dy);

			break;
		}
		case SDL_WINDOWEVENT:
		{
			if (current_event.window.event == SDL_WINDOWEVENT_RESIZED)
				windowsize_changed = true;
			break;
		}
		case SDL_MOUSEWHEEL:
		{
			if (current_event.wheel.y < 0)
				cam1->moveBack(0.1f);
			else if (current_event.wheel.y > 0)
				cam1->moveForward(0.1f);
			break;
		}
		default : 
			break;
	}
}
/*
int main(int argc, char *argv[])
{
	// Using OpenGL 3.1 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Initialize video subsystem
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);
	// Create window
	window = SDL_CreateWindow("IN4I24", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	// Create OpenGL context
	glContext = SDL_GL_CreateContext(window);

	// Initialize glew
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);

	// Setting up OpenGL shaders
	shader1 = new myShader("shaders/light.vert.glsl", "shaders/light.frag.glsl");

	// Read up the scene
	obj1 = new myObject3D();
	obj1->readMesh("apple.obj"); 
	obj1->computeNormals();       
 

	cam1 = new myCamera();

	SDL_GetWindowSize(window, &cam1->window_width, &cam1->window_height);

	// display loop
	while (!quit)
	{
		if (windowsize_changed)
		{
			SDL_GetWindowSize(window, &cam1->window_width, &cam1->window_height);
			windowsize_changed = false;
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, cam1->window_width, cam1->window_height);

		glm::mat4 projection_matrix = cam1->projectionMatrix( );
		shader1->setUniform("myprojection_matrix", projection_matrix);

		glm::mat4 view_matrix = cam1->viewMatrix();
		shader1->setUniform("myview_matrix", view_matrix);

		obj1->displayObject(shader1);
		obj1->displayNormals(shader1);

		shader1->setUniform("input_color", glm::vec4(1, 1, 1, 0));
		glPointSize(6.0f);
		glBegin(GL_POINTS);
		 glVertex3fv(&picked_point[0]);
  	    glEnd();

		SDL_GL_SwapWindow(window);

		SDL_Event current_event;
		while (SDL_PollEvent(&current_event) != 0)
			processEvents(current_event);
	}
	
	// Freeing resources before exiting.
	// Destroy window
	if (glContext) SDL_GL_DeleteContext(glContext);
	if (window) SDL_DestroyWindow(window);

	// Quit SDL subsystems
	SDL_Quit();

	return 0;
}*/

#include <stdio.h>  // this library is for standard input and output
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "glut.h"// this library is for glut the OpenGL Utility Toolkit

#define RAINSIZE 50
int winWidth = 1000, winHeight = 1000;
int counter = 0;
time_t t;
float rotationAngle = 0;

struct drop {
	float x = 400;
	float y = 400;
	float inc = 0.01;
	float radius = 5;
	float scale = 1.0;
	float rotationAngle = 0;
	float rotationInc = 1;
};

drop rain[RAINSIZE];

void initRain() {
	srand((unsigned)time(&t));
	for (int i = 0; i < RAINSIZE; i++) {
		rain[i].x = rand() % winWidth;
		rain[i].y = rand() % winHeight;
		rain[i].inc = 1.5 + (float)(rand() % 100) / 1000.0;
		rain[i].radius = (float)(rand() % 8);
		rain[i].scale = (float)(rand() % 20000) / 1000.0;
		rain[i].rotationAngle = (float)(rand() % 3000) / 1000.0;
		rain[i].rotationInc = (float)(rand() % 100) / 1000.0;
		if ((rand() % 100) > 50) {
			rain[i].rotationInc = -rain[i].rotationInc;
		}
	}
}

void drawParticleShape(int i) {
	glBegin(GL_POINTS);
	glVertex2d(rain[i].x, rain[i].y);
	glEnd();
	glBegin(GL_LINES);
	glVertex2d(rain[i].x, rain[i].y);
	glVertex2d(rain[i].x, rain[i].y + rain[i].radius * 2);
	glEnd();
}

void drawDrop(int i) {
	glColor3f(0.0, 0.0, 1.0);
	glLineWidth(2);
	drawParticleShape(i);
	rain[i].y -= rain[i].inc;
	if (rain[i].y < 0) {
		rain[i].y = winHeight;
	}
}

void drawRain() {
	for (int i = 0; i < RAINSIZE; i++) {
		drawDrop(i);
	}
}

// this is the initialisation function, called once only
void init() {
	glClearColor(0.0, 0.0, 0.0, 0.0); // set what colour you want the background to be
	glMatrixMode(GL_PROJECTION); // set the matrix mode, we will look at this later
	gluOrtho2D(0.0, winWidth, 0.0, winHeight);
}

float elapsedTime = 0, base_time = 0, fps = 0, frames;

// This is the display function it is called when ever you want to draw something
// all drawing should be called form here
void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	drawRain();

	glFlush();
	counter++;
	glutPostRedisplay();
}

// This is the idle function it is called whenever the program is idle
// This has the effect of repeatedly calling the display function
void idle() {
	//display();
}

int main(int argc, char** argv) {

	srand(1);
	initRain();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Computer Graphics - E4FI");
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	init();

	glutIdleFunc(idle);
	glutDisplayFunc(display);

	glutMainLoop();

	return 0;
}