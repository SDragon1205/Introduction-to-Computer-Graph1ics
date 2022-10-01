#define _USE_MATH_DEFINES
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Object.h"
#include "glut.h"
#include <math.h>
using namespace std;

class Vertex {
	public:
		float x;
		float y;
		float z;
		void normalize() {
			float magnitude;
			magnitude = sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
			*this = *this / magnitude;
		}
		Vertex operator + (Vertex const& obj) {
			Vertex temp;
			temp.x = this->x + obj.x;
			temp.y = this->y + obj.y;
			temp.z = this->z + obj.z;
			return temp;
		}
		Vertex operator - (Vertex const& obj) {
			Vertex temp;
			temp.x = this->x - obj.x;
			temp.y = this->y - obj.y;
			temp.z = this->z - obj.z;
			return temp;
		}
		Vertex operator / (float const& num) {
			Vertex temp;
			temp.x = this->x / num;
			temp.y = this->y / num;
			temp.z = this->z / num;
			return temp;
		}
		friend ostream& operator<<(ostream& os, const Vertex& v) {
			os << v.x << "," << v.y << "," << v.z;
			return os;
		}
};

int windowSize[2];
unsigned int texture;
int width, height, nrChannels;
unsigned char* pikachuTexture;
Object* Clock = new Object("clock.obj");
Object* Pikachu = new Object("Pikachu.obj");

void light();
void display();
void idle();
void reshape(GLsizei, GLsizei);
void InitTexture();
void LoadModel(Object*, bool is_Pikachu = false);
void keyboard(unsigned char key, int x, int y);
void DrawBase();

// default light color, can be use to reset the material color
GLfloat default_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f};
GLfloat default_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f};


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("HW1");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	InitTexture();
	glutMainLoop();
	return 0;
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
	glMaterialfv(GL_FRONT, GL_SPECULAR, light_specular);
	GLfloat mat_shininess[] = { 100 };
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

int minute_ro = -1;
int hour_ro = -1;
int r_ro = 0;
int p_ro = 0;
int m_ro = 0;
bool key_r = false;
bool key_p = false;
bool key_m = false;

void display()
{
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
	gluLookAt(0.0, 15.0, 40.0,// eye
		0.0, 0.0, 0.0,     // center
		0.0, 1.0, 0.0);    // up

	light();

	////TO DO : add model, translate, rotate, scale ,material
	//
	DrawBase();
	if (key_r)
	{
		r_ro++;
		r_ro %= 360;
	}
	if (key_p)
	{
		p_ro++;
		p_ro %= 360;
	}
	if (key_m)
	{
		m_ro += 2;
		m_ro %= 360;
	}

	glPushMatrix();
	glRotatef(m_ro, 0.0f, 1.0f, 0.0f);
	glTranslatef(14.0f, 0.0f, 0.0f);
	glRotatef(p_ro, 0.0f, 1.0f, 0.0f);
	glRotatef(45, 0.0f, 1.0f, 0.0f);
	glScalef(10.0f, 10.0f, 10.0f);
	LoadModel(Pikachu, true);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 7.5f, 0.0f);
	glScalef(0.08f, 0.08f, 0.08f);
	glRotatef(r_ro, 0.0f, 1.0f, 0.0f);
	LoadModel(Clock, false);
	glPopMatrix();

	GLfloat green_diffuse[] = { 0.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, green_diffuse);
	glPushMatrix();
	GLUquadric* hour_hand = gluNewQuadric();
	glTranslatef(0.0f, 7.5f, 0.0f);
	glRotatef(r_ro, 0.0f, 1.0f, 0.0f);
	hour_ro++;
	hour_ro %= 4320;
	glRotatef(hour_ro/12, 0.0f, 0.0f, -1.0f);
	glRotatef(90, -1.0f, 0.0f, 0.0f);
	gluCylinder(hour_hand, 0.3f, 0.3f, 3.0f, 30, 30);
	glPopMatrix();

	GLfloat pink_diffuse[] = { 1.0f, 0.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, pink_diffuse);
	glPushMatrix();
	GLUquadric* minute_hand = gluNewQuadric();
	glTranslatef(0.0f, 7.5f, 0.0f);
	glRotatef(r_ro, 0.0f, 1.0f, 0.0f);
	minute_ro++;
	minute_ro %= 360;
	glRotatef(minute_ro , 0.0f, 0.0f, -1.0f);
	glRotatef(90, 0.0f, 1.0f, 0.0f);
	gluCylinder(minute_hand, 0.3f, 0.3f, 5.0f, 30, 30);
	glPopMatrix();


	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
	windowSize[0] = w;
	windowSize[1] = h;
}

void idle() {
	glutPostRedisplay();
}

void InitTexture() {
	stbi_set_flip_vertically_on_load(true);
	pikachuTexture = stbi_load("Pikachu.png", &width, &height, &nrChannels, 0);
	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pikachuTexture);

	glDisable(GL_TEXTURE_2D);
}

void LoadModel(Object* Model,bool is_Pikachu) {
	if (is_Pikachu) {
		GLfloat pikachu_diffuse[] = { 1, 1, 0, 1 };
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, pikachu_diffuse);
		glEnable(GL_TEXTURE_2D);
	}
	for (size_t i = 0; i < Model->fNum; ++i)
	{
		glBegin(GL_TRIANGLES);
		for (size_t j = 0; j < 3; ++j)
		{
			if (is_Pikachu) {
				int idx = i * 3 * 2 + j * 2;
				glTexCoord2f(Model->texcoords[idx],Model->texcoords[idx+1]);
			}
			glNormal3fv(Model->nList[Model->faceList[i][j].nIndex].ptr);
			glVertex3fv(Model->vList[Model->faceList[i][j].vIndex].ptr);
		}
		glEnd();
	}
	if (is_Pikachu) {
		// set amibent back to default
		glMaterialfv(GL_FRONT, GL_AMBIENT, default_ambient);
		glDisable(GL_TEXTURE_2D);
	}
}

void keyboard(unsigned char key, int x, int y) {
	//// TO DO : implement the keyboard function
	//
	switch(key){
	case 'r':
		if (key_r)
		{
			key_r = false;
		}
		else
		{
			key_r = true;
		}
		break;
	case 'p':
		if (key_p)
		{
			key_p = false;
		}
		else
		{
			key_p = true;
		}
		break;
	case 'm':
		if (key_m)
		{
			key_m = false;
		}
		else
		{
			key_m = true;
		}
		break;
	}
}

void DrawBase() {
	//// TO DO : Draw base 
	//

	//// TO DO : draw the star polygon
	//

	// Draw the triangle with color #f72585
	GLfloat red_diffuse[] = { 247.0f / 255.0f , 37.0f / 255.0f , 133.0f / 255.0f , 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, red_diffuse);
	//GLfloat x1, y1, z1, x2, y2, z2;
	for (int i = 1; i <= 6; i++)
	{

		glPushMatrix();
		glRotatef(r_ro, 0.0f, 1.0f, 0.0f);
		glBegin(GL_POLYGON);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(10 * cos(M_PI / 3 * i), 0.0f, 10 * sin(M_PI / 3 * i));
		glVertex3f(10 * sqrt(3) * cos(M_PI / 3 * i + (M_PI / 6)), 0.0f, 10 * sqrt(3) * sin(M_PI / 3 * i + (M_PI / 6)));
		glVertex3f(10 * cos(M_PI / 3 * (i + 1)), 0.0f, 10 * sin(M_PI / 3 * (i + 1)));
		glEnd();
		glPopMatrix();

		Vertex v1, v2, v3;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, red_diffuse);
		glPushMatrix();
		glRotatef(r_ro, 0.0f, 1.0f, 0.0f);
		glBegin(GL_POLYGON);
		v2.x = 0.0f;
		v2.y = -1.0f;
		v2.z = 0.0f;
		v1.x = (10 * sqrt(3) * cos(M_PI / 3 * i + (M_PI / 6))) - (10 * cos(M_PI / 3 * i));
		v1.y = 0.0f;
		v1.z = (10 * sqrt(3) * sin(M_PI / 3 * i + (M_PI / 6))) - (10 * sin(M_PI / 3 * i));
		v3.x = (v1.y * v2.z) - (v2.y * v1.z);
		v3.y = (v2.x * v1.z) - (v1.x * v2.z);
		v3.z = (v1.x * v2.y) - (v2.x * v1.y);
		v3.normalize();
		glNormal3f(v3.x, v3.y, v3.z);
		glVertex3f(10 * cos(M_PI / 3 * i), 0.0f, 10 * sin(M_PI / 3 * i));
		glVertex3f(10 * cos(M_PI / 3 * i), -5.0f, 10 * sin(M_PI / 3 * i));
		glVertex3f(10 * sqrt(3) * cos(M_PI / 3 * i + (M_PI / 6)), -5.0f, 10 * sqrt(3) * sin(M_PI / 3 * i + (M_PI / 6)));
		glVertex3f(10 * sqrt(3) * cos(M_PI / 3 * i + (M_PI / 6)), 0.0f, 10 * sqrt(3) * sin(M_PI / 3 * i + (M_PI / 6)));
		glEnd();
		glPopMatrix();

		glMaterialfv(GL_FRONT, GL_DIFFUSE, red_diffuse);
		glPushMatrix();
		glRotatef(r_ro, 0.0f, 1.0f, 0.0f);
		glBegin(GL_POLYGON);
		v2.x = 0.0f;
		v2.y = -1.0f;
		v2.z = 0.0f;
		v1.x = (10 * cos(M_PI / 3 * (i + 1))) - (10 * sqrt(3) * cos(M_PI / 3 * i + (M_PI / 6)));
		v1.y = 0.0f;
		v1.z = (10 * sin(M_PI / 3 * (i + 1))) - (10 * sqrt(3) * sin(M_PI / 3 * i + (M_PI / 6)));
		v3.x = (v1.y * v2.z) - (v2.y * v1.z);
		v3.y = (v2.x * v1.z) - (v1.x * v2.z);
		v3.z = (v1.x * v2.y) - (v2.x * v1.y);
		v3.normalize();
		glNormal3f(v3.x, v3.y, v3.z);
		glVertex3f(10 * sqrt(3) * cos(M_PI / 3 * i + (M_PI / 6)), 0.0f, 10 * sqrt(3) * sin(M_PI / 3 * i + (M_PI / 6)));
		glVertex3f(10 * sqrt(3) * cos(M_PI / 3 * i + (M_PI / 6)), -5.0f, 10 * sqrt(3) * sin(M_PI / 3 * i + (M_PI / 6)));
		glVertex3f(10 * cos(M_PI / 3 * (i + 1)), -5.0f, 10 * sin(M_PI / 3 * (i + 1)));
		glVertex3f(10 * cos(M_PI / 3 * (i + 1)), 0.0f, 10 * sin(M_PI / 3 * (i + 1)));
		glEnd();
		glPopMatrix();
	}

	// Draw the hexagon with color #4cc9f0
	GLfloat blue_diffuse[] = { 114.0f / 255.0f , 201.0f / 255.0f , 240.0f / 255.0f , 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, blue_diffuse);
	glPushMatrix();
	glRotatef(r_ro, 0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);
	for (int i = 1; i <= 6; i++)
	{
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(10 * cos(M_PI / 3 * i), 0.0f, 10 * sin(M_PI / 3 * i));
	}
	glEnd();
	glPopMatrix();

	//// TO DO : draw the rectangle edge
	//
	

	glMaterialfv(GL_FRONT, GL_DIFFUSE, default_diffuse);
}