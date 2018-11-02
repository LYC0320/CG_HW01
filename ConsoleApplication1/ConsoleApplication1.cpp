// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include"../GL/glut.h"

#include <stdlib.h>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#pragma warning( disable : 4996 )


//IMPORTANT data type for image texture, you need to acesse its member for texture building
struct Image {
	unsigned long sizeX;
	unsigned long sizeY;
	char *data;
};
typedef struct Image Image;

//number of textures desired, you may want to change it to get bonus point
#define TEXTURE_NUM 7
//directories of image files
char* texture_name[TEXTURE_NUM] = {
	"../Resource/sun.bmp",
	"../Resource/mercury.bmp",
	"../Resource/earth.bmp",
	"../Resource/moon.bmp",
	"../Resource/jupiter.bmp",
	"../Resource/europa.bmp",
	"../Resource/Rock.bmp"
	//you may add additional image files here
};
//texture id array
GLuint texture[TEXTURE_NUM];


//time parameter for helping coordinate your animation, you may utilize it to help perform animation
#define deltaTime 100
double time = 0.0;
float angle = 0;

//light sorce parameter, no need to modify
//actually, modification of these parameter is forbidden in this homework
float LightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };//Light position
float LightAmb[] = { 0.0f, 0.0f, 0.0f, 1.0f };//Ambient Light Values
float LightDif[] = { 1.0f, 1.0f, 1.0f, 1.0f };//Diffuse Light Values
float LightSpc[] = { 1.0f, 1.0f, 1.0f, 1.0f };//Specular Light Values

											  //24-bit bmp loading function, no need to modify it
int ImageLoad(char *filename, Image *image) {
	FILE *file;
	unsigned long size; // size of the image in bytes.
	unsigned long i; // standard counter.
	unsigned short int planes; // number of planes in image (must be 1)
	unsigned short int bpp; // number of bits per pixel (must be 24)
	char temp; // temporary color storage for bgr-rgb conversion.
			   // make sure the file is there.
	if ((file = fopen(filename, "rb")) == NULL) {
		printf("File Not Found : %s\n", filename);
		return 0;
	}

	// seek through the bmp header, up to the width/height:
	fseek(file, 18, SEEK_CUR);
	// read the width
	if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {
		printf("Error reading width from %s.\n", filename);
		return 0;
	}

	//printf("Width of %s: %lu\n", filename, image->sizeX);
	// read the height
	if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {
		printf("Error reading height from %s.\n", filename);
		return 0;
	}
	//printf("Height of %s: %lu\n", filename, image->sizeY);
	// calculate the size (assuming 24 bits or 3 bytes per pixel).
	size = image->sizeX * image->sizeY * 3;
	// read the planes
	if ((fread(&planes, 2, 1, file)) != 1) {
		printf("Error reading planes from %s.\n", filename);
		return 0;
	}
	if (planes != 1) {
		printf("Planes from %s is not 1: %u\n", filename, planes);
		return 0;
	}
	// read the bitsperpixel
	if ((i = fread(&bpp, 2, 1, file)) != 1) {
		printf("Error reading bpp from %s.\n", filename);
		return 0;
	}
	if (bpp != 24) {
		printf("Bpp from %s is not 24: %u\n", filename, bpp);
		return 0;
	}
	// seek past the rest of the bitmap header.
	fseek(file, 24, SEEK_CUR);
	// read the data.
	image->data = (char *)malloc(size);
	if (image->data == NULL) {
		printf("Error allocating memory for color-corrected image data");
		return 0;
	}
	if ((i = fread(image->data, size, 1, file)) != 1) {
		printf("Error reading image data from %s.\n", filename);
		return 0;
	}
	for (i = 0; i<size; i += 3) { // reverse all of the colors. (bgr -> rgb)
		temp = image->data[i];
		image->data[i] = image->data[i + 2];
		image->data[i + 2] = temp;
	}
	// we're done.
	return 1;
}
//memory allocation and file reading for an Image datatype, no need to modify it
Image * loadTexture(char *filename) {
	Image *image;
	// allocate space for texture
	image = (Image *)malloc(sizeof(Image));
	if (image == NULL) {
		printf("Error allocating space for image");
		getchar();
		exit(0);
	}
	if (!ImageLoad(filename, image)) {
		getchar();
		exit(1);
	}
	return image;
}

void satellite()						//衛星函式
{
	GLUquadricObj* quad = gluNewQuadric();

	glPushMatrix();

	glPushMatrix();
	glTranslatef(0, 0, -1.5);
	glScalef(1, 1, 0.5);
	glPushMatrix();
	gluCylinder(quad, 1, 1, 6, 20, 20); //衛星圓柱
	glTranslatef(0, 0, 3);
	gluDisk(quad, 0, 1, 20, 20);        //圓柱蓋子
	glTranslatef(0, 0, -3);
	gluDisk(quad, 0, 1, 20, 20);		//圓柱蓋子
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();
	glScalef(1, 0.05, 0.5);
	glutSolidCube(6);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 2, 0);
	glRotatef(90, 1, 0, 0);
	glPushMatrix();
	gluCylinder(quad, 2, 0, 1, 20, 20);	//衛星圓錐
	gluDisk(quad, 0, 2, 20, 20);		//圓錐蓋子
	glPopMatrix();
	glPopMatrix();

	glPopMatrix();

}

//callback function for drawing a frame
void Display(void)
{


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);//set the view part of modelview matrix
	glLoadIdentity();

	gluLookAt(0, 0, 20, 0, 0, 0, 0, 1, 0);


	glLightfv(GL_LIGHT1, GL_POSITION, LightPos);//Set Light1 Position, this setting function should be at another place


	GLUquadricObj* quad = gluNewQuadric();//glu quadratic object for drawing shape like sphere and cylinder

										  //please draw the scene in the following region
										  //here are some example objects without texture
										  //please notice how they look like without rotation
										  //glDisable(GL_TEXTURE_2D);//when you draw something without texture, be sure to disable GL_TEXTURE_2D

	glDisable(GL_LIGHTING);

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texture[0]);

	glPushMatrix(); //sun
	glColor3f(1.0, 1.0, 1.0);
	gluQuadricTexture(quad, GLU_TRUE);
	gluSphere(quad, 2, 20, 20);
	glPopMatrix();



	glDisable(GL_TEXTURE_2D);

	glEnable(GL_LIGHTING);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[1]);

	glPushMatrix();
	glRotatef(40 * time, 0, 1, 0); //mercury公轉
	glTranslatef(3, 0, 0);

	glPushMatrix();
	glRotatef(150 * time, 0, 1, 0); //mercury自轉
	gluSphere(quad, 1, 20, 20);
	glPopMatrix();
	glPopMatrix();


	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[4]);

	glPushMatrix();
	glRotatef(45 * time, -1, 1, 0); //jupiter公轉
	glTranslatef(7, 7, 0);

	glPushMatrix();
	glTranslatef(0, 4, 0);
	glScalef(.2, .2, .2);		//衛星大小
	glColor3f(1, 1, 0);			//衛星顏色
	satellite();
	glPopMatrix();

	glPushMatrix();

	glRotatef(100 * time, 0, 1, 0); //jupiter自轉
	glColor3f(1, 1, 1);
	gluSphere(quad, 3, 20, 20);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[5]);

	glPushMatrix();
	glRotatef(200 * time, -1, 1, 0); //europa公轉
	glTranslatef(4, 4, 0);
	gluSphere(quad, 1, 20, 20);
	glPopMatrix();



	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[6]);

	glPushMatrix();
	glRotatef(150 * time, 0, 1, 0); //rock公轉
	glTranslatef(5, 0, 0);
	gluSphere(quad, 1, 20, 20);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);


	glPushMatrix();
	glRotatef(30 * time, 0.3, 1, 0); //地球公轉
	glTranslatef(-12, 2, 0);		 //地球公轉半徑

	glPushMatrix();
	glTranslatef(0, 3, 0);
	glScalef(.2, .2, .2);		//衛星大小
	glColor3f(0, 1, 1);			//衛星顏色
	satellite();
	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[2]);

	glPushMatrix(); //earth
	glRotatef(150 * time, 0.2, 1, 0); //地球自轉
	glScalef(1.1, 1, 1);
	glColor3f(1, 1, 1);
	gluSphere(quad, 2, 20, 20);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[3]);

	glPushMatrix(); //moon
	glRotatef(70 * time, 0, 1, 0); //月球公轉
	glTranslatef(-4, 0, 0);        //月球公轉半徑
	gluSphere(quad, 1, 20, 20);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();




	glutSwapBuffers();//swap the drawn buffer to the window
}

//callback funtion as a timer, no need to modify it
void Tick(int id) {
	double d = deltaTime / 1000.0;
	time += d;

	angle = angle + 0.05;

	glutPostRedisplay();
	glutTimerFunc(deltaTime, Tick, 0); // 100ms for time step size
}

//callback function when the window size is changed, no need to modify it
void WindowSize(int w, int h)
{
	glViewport(0, 0, w, h);							//changing the buffer size the same to the window size
	glMatrixMode(GL_PROJECTION);					//choose the projection matrix
	glLoadIdentity();
	gluPerspective(60.0, (double)w / (double)h, 1.0, 1000.0);//set the projection matrix as perspective mode
	glMatrixMode(GL_MODELVIEW);						//it is suggested that modelview matrix is chosen if nothing specific being performed
	glLoadIdentity();
}

//initialization for parameters of this program, you must perform something hereglDisable(GL_TEXTURE_2D);
void init()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);//set what color should be used when we clean the color buffer
	glEnable(GL_LIGHT1);//Enable Light1
	glEnable(GL_LIGHTING);//Enable Lighting
						  //***********

						  //***********
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmb);//Set Light1 Ambience
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDif);//Set Light1 Diffuse
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpc);//Set Light1 Specular
												//since a light source is also an object in the 3D scene, we set its position in the display function after gluLookAt()
												//you should know that straight texture mapping(without modifying shader) may not have shading effect
												//you need to tweak certain parameter(when building texture) to obtain a lit and textured object
	glShadeModel(GL_SMOOTH);//shading model

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);//you can choose which part of lighting model should be modified by texture mapping
	glEnable(GL_COLOR_MATERIAL);//enable this parameter to use glColor() as material of lighting model

								//please load all the textures here
								//use Image* loadTexture(file_directory) function to obtain char* data and size info.

	Image *sun = loadTexture(texture_name[0]);
	Image *mercury = loadTexture(texture_name[1]);
	Image *earth = loadTexture(texture_name[2]);
	Image *moon = loadTexture(texture_name[3]);
	Image *jupiter = loadTexture(texture_name[4]);
	Image *europa = loadTexture(texture_name[5]);
	Image *rock = loadTexture(texture_name[6]);

	glGenTextures(7, texture);

	glBindTexture(GL_TEXTURE_2D, texture[0]);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, sun->sizeX, sun->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, sun->data);
	//glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);


	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, mercury->sizeX, mercury->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, mercury->data);
	glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, earth->sizeX, earth->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, earth->data);
	glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, moon->sizeX, moon->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, moon->data);
	glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, jupiter->sizeX, jupiter->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, jupiter->data);
	glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, europa->sizeX, europa->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, europa->data);
	glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, texture[6]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, rock->sizeX, rock->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, rock->data);
	glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//glShadeModel(GL_FLAT);


}



int main(int argc, char *argv[])
{
	glutInit(&argc, argv);//glut function for simplifying OpenGL initialization
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);//demanding: double-framed buffer | RGB colorbuffer | depth buffer
	glutInitWindowPosition(100, 50);//set the initial window position
	glutInitWindowSize(1600, 800);//set the initial window size
								  //**************************
	glutCreateWindow("CG_HW1_0656632");
									   //**************************
	glutDisplayFunc(Display);//callback funtion for drawing a frame
	glutReshapeFunc(WindowSize);//callback function when the window size is changed
	glutTimerFunc(deltaTime, Tick, 0);//timer function
									  //you may want to write your own callback funtion for other events(not demanded nor forbidden)
	init();//self-defined initialization function for the elegance of your code

	glutMainLoop();
	return 0;
}