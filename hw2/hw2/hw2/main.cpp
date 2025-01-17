#define _USE_MATH_DEFINES
#define STB_IMAGE_IMPLEMENTATION
#include "Object.h"
#include "glew.h"
#include "glut.h"
#include "shader.h"
#include <math.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <thread>
#include <chrono>
#include "stb_image.h"
#include "Vertex.h"

using namespace std;

int windowSize[2];

void light();
void display();
void init();
void keyboard(unsigned char key, int x, int y);
void reshape(GLsizei, GLsizei);
void idle();

void shaderInit();
void textureInit();
void bindBuffer(Object* model);
void DrawSphere(float radius, float slice, float stack);
void drawModel(Object* model);
void LoadTexture(unsigned int& texture, const char* tFileName,int i);
void Sleep(int ms);

// feeling free to adjust below value to fit your computer efficacy.
#define Rotate_Speed 1
#define MAX_FPS 120

// timer for FPS control
clock_t Start, End;
float speed = 0;

Object* Pikachu = new Object("Pikachu.obj");
unsigned int Pikachu_texture;
unsigned int ball_texture;

vector<VertexAttribute> ball;

GLuint program;
GLuint vbo_pi, vbo_po;
GLuint vao_pi, vao_po;

int main(int argc, char** argv) {
	// set up OpenGL & window
	glutInit(&argc, argv);
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("HW2");

	// create & initial related data
	DrawSphere(3, 720, 360);
	glewInit();
	shaderInit();
	textureInit();
	bindBuffer(Pikachu);

	// bind OpenGL event function
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}

void reshape(GLsizei w, GLsizei h) {
	windowSize[0] = w;
	windowSize[1] = h;
}

void idle() {
	// FPS control
	clock_t CostTime = End - Start;
	float PerFrameTime = 1000.0 / MAX_FPS;
	if (CostTime < PerFrameTime) {
		Sleep(ceil(PerFrameTime) - CostTime);
	}
	glutPostRedisplay();
}

void light()
{
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_position[] = { 10.0, 10.0, 5.0, 0.0 };
	glShadeModel(GL_SMOOTH);
	// z buffer enable
	glEnable(GL_DEPTH_TEST);
	// enable lighting
	glEnable(GL_LIGHTING);
	// set light property
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
}

bool key_s = false;
int ro = 0;

void keyboard(unsigned char key, int x, int y) {
	// TODO : keyboard function , press key ��s�� to start/stop all models rotation around y axis.
	// Hint : 
	//		# The concept is as same as keyboard function in HW1
	switch (key) {
	case 's':
		if (key_s)
		{
			key_s = false;
		}
		else
		{
			key_s = true;
		}
		break;
	}

}

void display() {
	Start = clock();
	init();
	light();
	drawModel(Pikachu);
	End = clock();

	glutPostRedisplay();
	glutSwapBuffers();
}

void init() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// viewport transformation
	glViewport(0, 0, windowSize[0], windowSize[1]);

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)windowSize[0] / (GLfloat)windowSize[1], 1.0, 1000.0);

	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(7.5, 5.0, 10,// eye
		0.0, 0.0, 0.0,     // center
		0.0, 1.0, 0.0);    // up
}

void shaderInit() {
	// TODO : create the shader & program
	// Hint : 
	//		# create the shader file named "filename.vert" & "filename.frag" under the "/hw2/dll/Shaders/"
	//		# use the function defined in shaer.h to create shader
	GLuint vert = createShader("Shaders/hw1.vert", "vertex");
	GLuint frag = createShader("Shaders/hw1.frag", "fragment");
	program = createProgram(vert, frag);
}

void textureInit() {
	glEnable(GL_TEXTURE_2D);
	// bind Pikachu texture with GL_TEXTURE0
	LoadTexture(Pikachu_texture, "Pikachu.png",0);
	// bind Pokeball texture with GL_TEXTURE1
	LoadTexture(ball_texture, "Pokeball.png",1);
}

void bindBuffer(Object* model) {
	// TODO : use VAO & VBO to buffer the vertex data which will be passed to shader
	// Hint :
	//		# use "VertexAttribute" defined in Vertex.h to store the infomation of vertex needed in shader
	//		# the Pikachu model data is stored in function paramete "model" (or using global variable "Pikachu")
	//		# the pokeball vertex is stored in global variabl "ball"
	//		# see Object class detail in "Object.h" to pick up needed data to buffer
	// Generate a new buffer object

	vector<VertexAttribute> va;
	VertexAttribute temp;
	int max = Pikachu->positions.size();
	for (int it = 0, it2 = 0; it < max; it += 3, it2 += 2)
	{
		temp.setPosition(Pikachu->positions[it], Pikachu->positions[it + 1], Pikachu->positions[it + 2]);

		temp.setTexcoord(Pikachu->texcoords[it2], Pikachu->texcoords[it2+1]);

		va.push_back(temp);
	}

	glGenVertexArrays(1, &vao_pi);
	glBindVertexArray(vao_pi);
	glGenBuffers(1, &vbo_pi);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_pi);
	glBufferData(GL_ARRAY_BUFFER, va.size() * sizeof(VertexAttribute), &va[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex) + sizeof(GLfloat) * 3, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex) + sizeof(GLfloat) * 3, (void*)sizeof(Vertex));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	glGenVertexArrays(1, &vao_po);
	glBindVertexArray(vao_po);
	glGenBuffers(1, &vbo_po);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_po);
	glBufferData(GL_ARRAY_BUFFER, ball.size() * sizeof(VertexAttribute), &ball[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex) + sizeof(GLfloat) * 3, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex) + sizeof(GLfloat) * 3, (void*)sizeof(Vertex));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void DrawSphere(float radius, float slice, float stack) {
	// TODO : calculate the texture coordinate of sphere
	// Hint : 
	//		# read the code below to know how to iteratively compute the vertex position of sphere by sphere coordinate
	//		# find the mathematical way to iteratively calculate the u , v textur coordinate of a vertex on sphere
	float theta, phi, xy_step = 360 / slice, z_step = 180 / stack;
	Vertex vert;
	float u, v;
	for (phi = -90; phi <= 90; phi += z_step) {
		VertexAttribute temp;
		for (theta = 0; theta <= 360; theta += xy_step) {
			vert.x = radius * sin(theta * M_PI / 180) * cos(phi * M_PI / 180);
			vert.y = radius * cos(theta * M_PI / 180) * cos(phi * M_PI / 180);
			vert.z = radius * sin(phi * M_PI / 180);
			temp.setPosition(vert);

			u = theta / 360;
			v = (phi + 90) / 180;
			temp.setTexcoord(u, v);

			ball.push_back(temp);

			vert.x = radius * sin(theta * M_PI / 180) * cos((phi + z_step) * M_PI / 180);
			vert.y = radius * cos(theta * M_PI / 180) * cos((phi + z_step) * M_PI / 180);
			vert.z = radius * sin((phi + z_step) * M_PI / 180);
			temp.setPosition(vert);

			u = (theta + xy_step) / 360;
			v = (phi + 90 + z_step) / 180;
			temp.setTexcoord(u, v);

			ball.push_back(temp);
		}
	}
}


void drawModel(Object* model) {
	// TODO : draw all the models on correct position & send modelview and projection matrix to shader
	// Hint :
	//		# move the model by glRotate,glTranslate , glScale , glPush , glPop ... and so on
	//		# pass modelview matrix to shader after transformation
	//		# use program , VAO , texture , "glDrawArrays" ... and so on to render model
	if (key_s)
	{
		ro += 1;
		ro %= 360;
	}

	glPushMatrix();
	glRotatef(ro+25, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, 0.0f);
	glScalef(5.0f, 5.0f, 5.0f);

	GLfloat pmtx[16];
	GLfloat mmtx[16];

	// get current modelview & projection matrix used in OpenGL rendering
	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);

	// get the "location" (It's similar to pointer) of uniform variable in shader
	GLint pmatLoc = glGetUniformLocation(program, "Projection");
	GLint mmatLoc = glGetUniformLocation(program, "ModelView");

	// using shader
	glUseProgram(program);

	glUniform1i(glGetUniformLocation(program, "Texture") , 0);

	glBindVertexArray(vao_pi);

	// pass the projection matrix into vertex shader
	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	// pass the modelview matrix into vertex shader
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);

	// draw the vertex stored in buffer 
	glDrawArrays(GL_TRIANGLES, 0, Pikachu->positions.size() / 3);
	glUseProgram(0);
	glPopMatrix();
	//glutSwapBuffers();

	glPushMatrix();
	glRotatef(ro+45, 0.0f, 1.0f, 0.0f);
	glTranslatef(3.0f, 0.0f, -3.0f);
	glRotatef(270, 1, 0, 0);

	GLfloat pmtx2[16];
	GLfloat mmtx2[16];

	// get current modelview & projection matrix used in OpenGL rendering
	glGetFloatv(GL_PROJECTION_MATRIX, pmtx2);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx2);

	// get the "location" (It's similar to pointer) of uniform variable in shader
	GLint pmatLoc2 = glGetUniformLocation(program, "Projection");
	GLint mmatLoc2 = glGetUniformLocation(program, "ModelView");
	// using shader
	glUseProgram(program);

	glUniform1i(glGetUniformLocation(program, "Texture"), 1);

	glBindVertexArray(vao_po);

	// pass the projection matrix into vertex shader
	glUniformMatrix4fv(pmatLoc2, 1, GL_FALSE, pmtx2);
	// pass the modelview matrix into vertex shader
	glUniformMatrix4fv(mmatLoc2, 1, GL_FALSE, mmtx2);

	// draw the vertex stored in buffer 
	glDrawArrays(GL_TRIANGLE_STRIP, 0, ball.size());
	glUseProgram(0);

	glPopMatrix();
}

// i indicate the texture unit number
void LoadTexture(unsigned int& texture, const char* tFileName, int i) {
	// TODO : generating the texture with texture unit
	// Hint : 
	//		# glActiveTexture() , glGenTextures() and so on ...
	//		# GL_TEXTUREi = (GL_TEXTURE0 + i)
	//      # make sure one texture unit only binds one texture
	//		# It's different with VAO,VBO that texture don't need to unbind. (Just active different texture unit)
	glActiveTexture(GL_TEXTURE0+i);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(tFileName, &width, &height, &nrChannels, 0);

	if (data)
	{
	// TODO : use image data to generate the texture here
	// Hint :
	//		# glTexImage2D()
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		cout << "Failed to load texture" << endl;
	}
	stbi_image_free(data);
}

void Sleep(int ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
