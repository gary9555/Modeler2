#ifndef TEX_H
#define TEX_H

#include <FL/gl.h>
#include <FL/glut.H>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using namespace std;

class Texture{
public:
	GLuint loadBMP_custom(const char * imagepath);
};

#endif