/**
 * landscape.h
 * Header for landscape.c
 * Created in 2008 for 433-380 Project 2.
 * Written by Josh Torrance with additions by Francis Byrne.
 */

/***************** Libraries *****************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/* GLUT headers */
#include <GL/Glut.h>

/* Maths library - remember to use -lm if building with GCC */
#include <math.h>


/***************** Type Defintitions *****************/
typedef float colourVector[4];	// red, green, blue, alpha
typedef double Vector[3];	// x, y (height), z

typedef struct{
	double			height;
	colourVector	rgba;
	colourVector	specular_rgba;
	Vector	normal;
}mapVertex;

typedef mapVertex **map;

/***************** Constants *****************/
#define MAXIMUM 200
	// The maximum value for heights - MAXIMUM cannot be zero
#define MINIMUM 0
	// The minimum value for heights
#define RANGE (MAXIMUM-MINIMUM)
#define RAND_FACTOR 0.6
	// Determines the magnitude of the displacement in plasma_fractal
#define RAND_FACTOR_2 0.1
	// Determines the magnitude of the displacement for non central side in plasma fractal
#define RAND_HEIGHT_DISPLACEMENT 0.5
	// Determines the magnitude of the random element of the colour

/***************** Globals *****************/

/***************** Function Prototypes *****************/
map generate_map(int size);
void set_height(int size, map landscape);
void set_terrain(map landscape, int size);
void set_normals(map landscape, int size);
void plasma_fractal(map landscape, int size,
		int x0, int x1, int y0, int y1);
double random_displacement(int x0, int x1);
void normalize_vector(double *vector);
double random_fraction(void);
double magnitude(double *vector);
void draw_landscape(map landscape, int size);

/***************** Error Messages *****************/
#define ERROR_LANDSCAPE_POINTER "Failed to create map in 'generate_map' [landscape.c]\n"