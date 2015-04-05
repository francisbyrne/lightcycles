/**
 * landscape.c
 * Functions for creating a random 3D terrain using the plasma fractal algorithm.
 * Created in 2008 for 433-380 Project 2.
 * Written by Josh Torrance with additions by Francis Byrne.
 */

/***************** Header File *****************/
#include "landscape.h"
#include "utility.h"

/***************** Functions *****************/
/*
 * This function generates a height map of a given size and returns it.
 * The height map includes a height, colour and normal
 * Assume that size is greater than 1
 */
map
generate_map(int size)
{
	int i;
	// Create the Map
	map landscape;
	landscape = (map)calloc(size, sizeof(mapVertex*));
	check_pointer(landscape, ERROR_LANDSCAPE_POINTER);

	for ( i=0 ; i<size ; i++ )
	{
		landscape[i] = (mapVertex *)calloc(size, sizeof(mapVertex));
		check_pointer(landscape[i], ERROR_LANDSCAPE_POINTER);
	}

	// Generate Heights - using the plasma fractal algorithm
	set_height(size, landscape);

	// Allocate Colours
	set_terrain(landscape, size);

	// Calculate Normals
	set_normals(landscape, size);

	// Return the Map
	return landscape;

}

/* This function generates heights for a map using the plasma fractal algorithm
 * The function assumes that srand has already been called */
void
set_height(int size, map landscape)
{
	// set the heights of the corner points
	((landscape[0])[0]).height =
				MINIMUM + RANGE*((double)rand()/((double)(RAND_MAX)+(double)(1)));
	((landscape[0])[size-1]).height =
				MINIMUM + RANGE*((double)rand()/((double)(RAND_MAX)+(double)(1)));
	((landscape[size-1])[0]).height =
				MINIMUM + RANGE*((double)rand()/((double)(RAND_MAX)+(double)(1)));
	((landscape[size-1])[size-1]).height =
				MINIMUM + RANGE*((double)rand()/((double)(RAND_MAX)+(double)(1)));

	// Call the recursive plasma fractal algorithm
	plasma_fractal(landscape, size, 0, size-1, 0, size-1);
}

/* This function implements the plasma fractal algorithm recursively
 * Only square maps will work. */
void
plasma_fractal(map landscape, int size,
				int x0, int x1, int y0, int y1)
{
	int dx = x1 - x0;	// dx = dy since only squars are being mapped
	int midX = x0+dx/2, midY = y0+dx/2;
	double temp;

	if ( dx == 1 )		// x is at the base, we're using squares so dy is 1 too
	{
		// Base Case - we can go no lower
		return;
	} else {			// dx & dy > 1
			if ( dx%2 == 0 )	// dx is even
			{
				// mid point for the left side
				(landscape[x0])[midY].height =
						( (landscape[x0])[y0].height + (landscape[x0])[y1].height ) /2;
				// mid point for the right side
				(landscape[x1][midY]).height =
						( (landscape[x1])[y0].height + (landscape[x1])[y1].height ) /2;
				// mid point for the bottom side
				(landscape[midX][y0]).height =
						( (landscape[x0])[y0].height + (landscape[x1])[y0].height ) /2;
				// mid point for the top side
				(landscape[midX][y1]).height =
						( (landscape[x0])[y1].height + (landscape[x1])[y1].height ) /2;
				// mid point (centre)
				temp =
/* average of */		( (landscape[x0])[y0].height + (landscape[x0])[y1].height +
/* the heights */		  (landscape[x1])[y0].height + (landscape[x1])[y1].height ) / 4
/* + displacement */	+ random_displacement(x0, x1);

				// Does the new value break the bounds, MINIMUM & MAXIMUM?
				if ( temp > MAXIMUM )
				{
					((landscape[midX])[midY]).height = MAXIMUM;
				}
				else if ( temp < MINIMUM )
				{
					((landscape[midX])[midY]).height = MINIMUM;
				}
				else
				{
					((landscape[midX])[midY]).height = temp;
				}

				// Now recurse on the four sub squares
				plasma_fractal(landscape, size, x0, midX, y0, midY); // bottom left
				plasma_fractal(landscape, size, midX, x1, y0, midY); // bottom right
				plasma_fractal(landscape, size, x0, midX, midY, y1); // top left
				plasma_fractal(landscape, size, midX, x1, midY, y1); // top right
			} else {				// dx is odd
				// midpoints for the left side
				temp = ( (landscape)[x0][y0].height + (landscape)[x0][y1].height ) /2;
				((landscape)[x0][midY]).height = temp + random_displacement(0, 2);
				((landscape)[x0][midY+1]).height = temp + random_displacement(0, 2);

				// midpoints for the right side
				temp = ( (landscape)[x1][y0].height + (landscape)[x1][y1].height ) /2;
				((landscape)[x1][midY]).height = temp + random_displacement(0, 2);
				((landscape)[x1][midY+1]).height = temp + random_displacement(0, 2);

				// midpoints for the bottom side
				temp = ( (landscape)[x0][y0].height + (landscape)[x1][y0].height ) /2;
				((landscape)[midX][y0]).height = temp + random_displacement(0, 2);
				((landscape)[midX+1][y0]).height = temp + random_displacement(0, 2);

				// midpoints for the top side
				temp = ( (landscape)[x0][y1].height + (landscape)[x1][y1].height ) /2;
				((landscape)[midX][y1]).height = temp + random_displacement(0, 2);
				((landscape)[midX+1][y1]).height = temp + random_displacement(0, 2);

				// midpoint for the centre
				temp =
/* average of */		( (landscape)[x0][y0].height + (landscape)[x0][y1].height +
/* the heights */		  (landscape)[x1][y0].height + (landscape)[x1][y1].height ) / 4
/* + displacement */	+ random_displacement(x0, x1);

				// Does the new value break the bounds, MINIMUM & MAXIMUM?
				if ( temp > MAXIMUM )
				{
					(landscape)[midX][midY].height = MAXIMUM - random_displacement(0, 1);
					(landscape)[midX][midY+1].height = MAXIMUM - random_displacement(0, 1);
					(landscape)[midX+1][midY].height = MAXIMUM - random_displacement(0, 1);
					(landscape)[midX+1][midY+1].height = MAXIMUM - random_displacement(0, 1);
				} else if ( temp < MINIMUM )
				{
					(landscape)[midX][midY].height = MINIMUM + random_displacement(0, 1);
					(landscape)[midX][midY+1].height = MINIMUM + random_displacement(0, 1);
					(landscape)[midX+1][midY].height = MINIMUM + random_displacement(0, 1);
					(landscape)[midX+1][midY+1].height = MINIMUM + random_displacement(0, 1);
				} else {
					((landscape)[midX][midY]).height = temp + random_displacement(0, 1);
					((landscape)[midX][midY+1]).height = temp + random_displacement(0, 1);
					((landscape)[midX+1][midY]).height = temp + random_displacement(0, 1);
					((landscape)[midX+1][midY+1]).height = temp + random_displacement(0, 1);
				}

				// Now recurse on the four sub squares
				plasma_fractal(landscape, size, x0, midX, y0, midY); // bottom left
				plasma_fractal(landscape, size, midX+1, x1, y0, midY); // bottom right
				plasma_fractal(landscape, size, x0, midX, midY+1, y1); // top left
				plasma_fractal(landscape, size, midX+1, x1, midY+1, y1); // top right
			}
	}

}

/* This function sets the colours (in rgba) for a map.
 * The colour depends on the height of the vertex. */
void
set_terrain(map landscape, int size)
{
	int i, j;
	double cur_height;//, random = (random_fraction() - 0.5)*
						//RAND_HEIGHT_DISPLACEMENT*RANGE;
		// a random number between (-/+)0.5*RAND_HEIGHT_DISPLACEMENT*RANGE

	for ( i=0 ; i<size ; i++ )
	{
		for ( j=0 ; j<size ; j++ )
		{
			cur_height = landscape[i][j].height;// + random;
				// Add the random displacement so to give a mottled effect
		
			if ( cur_height < ( 0.2*MAXIMUM ) )
			{
				// level 1 - blue water: now grey rocks
				landscape[i][j].rgba[0] = 0.4*cur_height/(0.2*MAXIMUM);
				landscape[i][j].rgba[1] = 0.5*cur_height/(0.2*MAXIMUM);
				landscape[i][j].rgba[2] = 0.4*cur_height/(0.2*MAXIMUM);

				landscape[i][j].specular_rgba[0] = 0.2;
				landscape[i][j].specular_rgba[1] = 0.2;
				landscape[i][j].specular_rgba[2] = 0.2;

			} else if ( cur_height < ( 0.3*MAXIMUM ) )
			{
				// level 2 - yellow sand
				landscape[i][j].rgba[0] = 0.8*(0.3*MAXIMUM - cur_height)/(0.1*MAXIMUM);
				landscape[i][j].rgba[1] = 0.8 - 0.2*(cur_height - 0.2*MAXIMUM)/(0.1*MAXIMUM);
				landscape[i][j].rgba[2] = 0.0;

				landscape[i][j].specular_rgba[0] = 0.0;
				landscape[i][j].specular_rgba[1] = 0.0;
				landscape[i][j].specular_rgba[2] = 0.0;
			} else if ( cur_height < ( 0.5*MAXIMUM ) )
			{
				// level 3 - green grass
				landscape[i][j].rgba[0] = 0.7*(cur_height-0.3*MAXIMUM)/(0.2*MAXIMUM);
				landscape[i][j].rgba[1] = 0.6 - 0.1*(cur_height-0.3*MAXIMUM)/(0.2*MAXIMUM);
				landscape[i][j].rgba[2] = 0.5*(cur_height-0.3*MAXIMUM)/(0.2*MAXIMUM);

				landscape[i][j].specular_rgba[0] = 0.0;
				landscape[i][j].specular_rgba[1] = 0.0;
				landscape[i][j].specular_rgba[2] = 0.0;
			} else if ( cur_height < ( 0.9*MAXIMUM ) )
			{
				// level 4 - brown rocks
				landscape[i][j].rgba[0] = 0.7 - 0.2*(cur_height-0.5*MAXIMUM)/(0.4*MAXIMUM);
				landscape[i][j].rgba[1] = 0.5;
				landscape[i][j].rgba[2] = 0.5;

				landscape[i][j].specular_rgba[0] = 0.0;
				landscape[i][j].specular_rgba[1] = 0.0;
				landscape[i][j].specular_rgba[2] = 0.0;
			} else
			{
				// level 6 - white snow
				landscape[i][j].rgba[0] = 0.7;
				landscape[i][j].rgba[1] = 0.7;
				landscape[i][j].rgba[2] = 0.7;

				landscape[i][j].specular_rgba[0] = 0.7;
				landscape[i][j].specular_rgba[1] = 0.7;
				landscape[i][j].specular_rgba[2] = 0.7;
			}

			// alpha values
			landscape[i][j].rgba[3] = 1;
			landscape[i][j].specular_rgba[3] = 1;
		}
	}
}

/* This function sets the normals of a map */
void
set_normals(map landscape, int size)
{
	int i, j;
	double h0, ha, hb;
	Vector n0, n1, n2, n3;

	// ***** The bottom row *****
	// Left vertex
	h0 = landscape[0][0].height;
	ha = landscape[1][0].height;
	hb = landscape[0][1].height;

	landscape[0][0].normal[0] = (0)*(hb-h0)-(ha-h0)*(1);
	landscape[0][0].normal[1] = (1)*(1)-(0)*(0);
	landscape[0][0].normal[2] = (ha-h0)*(0)-(1)*(hb-h0);

	for ( j=1 ; j<size-1 ; j++ )
	{	// Middle Vertices - final normal = average of 2 normals
		// First normal
		h0 = landscape[0][j].height;
		ha = landscape[0][j-1].height;
		hb = landscape[1][j].height;
		n0[0] = (ha-h0)*(1) - (0)*(hb-h0);
		n0[1] = (0)*(0) - (-1)*(1);
		n0[2] = (-1)*(hb-h0) - (ha-h0)*(0);

		normalize_vector(n0);

		// Second normal
		ha = landscape[1][j].height;
		hb = landscape[0][j+1].height;
		n1[0] = (ha-h0)*(0) - (1)*(hb-h0);
		n1[1] = (1)*(1) - (0)*(0);
		n1[2] = (0)*(hb-h0) - (ha-h0)*(1);

		normalize_vector(n1);
		
		// Final normal
		landscape[0][j].normal[0] = ( n0[0] + n1[0] ) / 2;
		landscape[0][j].normal[1] = ( n0[1] + n1[1] ) / 2;
		landscape[0][j].normal[2] = ( n0[2] + n1[2] ) / 2;

	}
	
	// Right vertex
	h0 = landscape[0][size-1].height;
	ha = landscape[0][size-2].height;
	hb = landscape[1][size-1].height;

	landscape[0][size-1].normal[0] = (ha-hb)*(1)-(0)*(hb-h0);
	landscape[0][size-1].normal[1] = (0)*(0)-(-1)*(1);
	landscape[0][size-1].normal[2] = (-1)*(hb-h0)-(ha-hb)*(0);

	// ***** The middle rows *****
	for ( i=1 ; i<size-1 ; i++ )
	{
		// Left vertex - average of 2 normals
		// First normal
		h0 = landscape[i][0].height;
		ha = landscape[i][1].height;
		hb = landscape[i-1][0].height;

		n0[0] = (ha-h0)*(-1) - (0)*(hb-h0);
		n0[1] = (0)*(0) - (1)*(-1);
		n0[2] = (1)*(hb-h0) - (ha-h0)*(0);

		normalize_vector(n0);

		// Second Normal
		ha = landscape[i+1][0].height;
		hb = landscape[i][1].height;

		n1[0] = (ha-h0)*(0) - (1)*(hb-h0);
		n1[1] = (1)*(1) - (0)*(0);
		n1[2] = (0)*(hb-h0) - (ha-h0)*(1);

		normalize_vector(n1);

		// Final Normal
		landscape[i][0].normal[0] = ( n0[0] + n1[0] ) / 2;
		landscape[i][0].normal[1] = ( n0[1] + n1[1] ) / 2;
		landscape[i][0].normal[2] = ( n0[2] + n1[2] ) / 2;


		for ( j=1 ; j<size-1 ; j++ )
		{	// Middle Vertices - final normal = average of 4 normals
			h0 = landscape[i][j].height;
			// First normal
			ha = landscape[i-1][j].height;
			hb = landscape[i][j-1].height;

			n0[0] = (ha-h0)*(0) - (-1)*(hb-h0);
			n0[1] = (-1)*(-1) - (0)*(0);
			n0[2] = (0)*(hb-h0) - (ha-h0)*(-1);

			normalize_vector(n0);

			// Second Normal
			ha = landscape[i][j+1].height;
			hb = landscape[i-1][j].height;

			n1[0] = (ha-h0)*(-1) - (0)*(hb-h0);
			n1[1] = (0)*(0) - (1)*(-1);
			n1[2] = (1)*(hb-h0) - (ha-h0)*(0);

			normalize_vector(n1);

			// Third Normal
			ha = landscape[i][j-1].height;
			hb = landscape[i+1][j].height;

			n2[0] = (ha-h0)*(1) - (0)*(hb-h0);
			n2[1] = (0)*(0) - (-1)*(1);
			n2[2] = (-1)*(hb-h0) - (ha-h0)*(0);

			normalize_vector(n2);

			// Forth Normal
			ha = landscape[i+1][j].height;
			hb = landscape[i][j+1].height;

			n3[0] = (ha-h0)*(0) - (1)*(hb-h0);
			n3[1] = (1)*(1) - (0)*(0);
			n3[2] = (0)*(hb-h0) - (ha-h0)*(1);

			normalize_vector(n3);

			// Final Normal
			landscape[i][j].normal[0] = ( n0[0] + n1[0] + n2[0] + n3[0] ) / 4;
			landscape[i][j].normal[1] = ( n0[1] + n1[1] + n2[1] + n3[1] ) / 4;
			landscape[i][j].normal[2] = ( n0[2] + n1[2] + n2[2] + n3[2] ) / 4;

		}
		// Right vertex - average of 2 normals
		// First normal
		h0 = landscape[i][size-1].height;
		ha = landscape[i-1][size-1].height;
		hb = landscape[i][size-2].height;

		n0[0] = (ha-h0)*(0) - (-1)*(hb-h0);
		n0[1] = (-1)*(-1) - (0)*(0);
		n0[2] = (0)*(hb-h0) - (ha-h0)*(-1);

		normalize_vector(n0);

		// Second Normal
		ha = landscape[i][size-2].height;
		hb = landscape[i+1][size-1].height;

		n1[0] = (ha-h0)*(1) - (0)*(hb-h0);
		n1[1] = (0)*(0) - (-1)*(1);
		n1[2] = (-1)*(hb-h0) - (ha-h0)*(0);

		normalize_vector(n1);

		// Final Normal
		landscape[i][size-1].normal[0] = ( n0[0] + n1[0] ) / 2;
		landscape[i][size-1].normal[1] = ( n0[1] + n1[1] ) / 2;
		landscape[i][size-1].normal[2] = ( n0[2] + n1[2] ) / 2;

	}

	// ***** The top row *****
	// Left vertex
	h0 = landscape[size-1][0].height;
	ha = landscape[size-1][1].height;
	hb = landscape[size-2][0].height;

	landscape[size-1][0].normal[0] = (ha-h0)*(-1) - (0)*(hb-h0);
	landscape[size-1][0].normal[1] = (0)*(0) - (1)*(-1);
	landscape[size-1][0].normal[2] = (1)*(hb-h0) - (ha-h0)*(0);

	for ( j=1 ; j<size-1 ; j++ )
	{	// Middle Vertices
		// First normal
		h0 = landscape[size-1][j].height;
		ha = landscape[size-2][j].height;
		hb = landscape[size-1][j-1].height;
		n0[0] = (ha-h0)*(0) - (-1)*(hb-h0);
		n0[1] = (-1)*(-1) - (0)*(0);
		n0[2] = (0)*(hb-h0) - (ha-h0)*(-1);

		normalize_vector(n0);

		// Second normal
		ha = landscape[size-1][j+1].height;
		hb = landscape[size-2][j].height;
		n1[0] = (ha-h0)*(-1) - (0)*(hb-h0);
		n1[1] = (0)*(0) - (1)*(-1);
		n1[2] = (1)*(hb-h0) - (ha-h0)*(0);


		normalize_vector(n1);
		
		// Final normal
		landscape[size-1][j].normal[0] = ( n0[0] + n1[0] ) / 2;
		landscape[size-1][j].normal[1] = ( n0[1] + n1[1] ) / 2;
		landscape[size-1][j].normal[2] = ( n0[2] + n1[2] ) / 2;

	}

	// Right vertex
	h0 = landscape[size-1][size-1].height;
	ha = landscape[size-2][size-1].height;
	hb = landscape[size-1][size-2].height;

	landscape[size-1][size-1].normal[0] = (ha-h0)*(0) - (-1)*(hb-h0);
	landscape[size-1][size-1].normal[1] = (-1)*(-1) - (0)*(0);
	landscape[size-1][size-1].normal[2] = (0)*(hb-h0) - (ha-h0)*(-1);

}

/* This function calculates the random displacement for set heights
 * The displacement depends on the distance between x0 and x1 */
double
random_displacement(int x0, int x1)
{
	return ((double)x1-(double)x0) * RAND_FACTOR *
							( random_fraction() - 0.5 );
}

/* This function normalizes a length 3 vector. Function does not check length! */
void
normalize_vector(double *vector)
{
	double m = magnitude(vector);

	if (magnitude != 0)
	{	// Lets not divide by zero!
		vector[0] = vector[0]/m;
		vector[1] = vector[1]/m;
		vector[2] = vector[2]/m;
	}
}

/* This function returns the magnitude of a vector */
double
magnitude(double *vector)
{
	return sqrt(vector[0]*vector[0] +
				 vector[1]*vector[1] +
				 vector[2]*vector[2] );
}

/* This function returns a random double between 0 and 1. */
double
random_fraction(void)
{
	return (double)rand() / ((double)(RAND_MAX)+(double)(1));
}

/* This function will draw a map in openGL */
void
draw_landscape(map landscape, int size)
{
	int i, j;
	GLfloat waterSpec[4] = {0.2,0.2,0.2,0.3};

	for ( i=0 ; i < size-1 ; i++ )
	{
		// Start drawing a triangle strip
		glBegin(GL_TRIANGLE_STRIP);

		for ( j=0 ; j < size ; j++ )
		{
			// first vertex
			glColor4fv(landscape[i][j].rgba);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, landscape[i][j].specular_rgba);
			glNormal3f(landscape[i][j].normal[0], landscape[i][j].normal[1], landscape[i][j].normal[2]);
			glVertex3d((double)j, landscape[i][j].height, (double)i);

			// second vertex
			glColor4fv(landscape[i+1][j].rgba);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, landscape[i+1][j].specular_rgba);
			glNormal3f(landscape[i+1][j].normal[0], landscape[i+1][j].normal[1], landscape[i+1][j].normal[2]);
			glVertex3d((double)j, landscape[i+1][j].height, (double)(i+1));

		}

		// Finish drawing landscape
		glEnd();
	}

	// Draw Edges
		// colour for the edges
	glColor4f(.3,.2,.2,1);

		// bottom edge
	glBegin(GL_TRIANGLE_STRIP);
//	glNormal3f(0,0,-1);
	i=0;
	for ( j=0 ; j<size ; j++ )
	{
		glVertex3d( (double)j, 0, (double)i );
		if ( landscape[i][j].height < 0.2*MAXIMUM )
		{
			glVertex3d( (double)j, 0.2*MAXIMUM, (double)i );
		} else {
			glVertex3d( (double)j, landscape[i][j].height, (double)i );
		}
	}
	glEnd();

		//left edge
	glBegin(GL_TRIANGLE_STRIP);
//	glNormal3f(0,-1,0);
	j=0;
	for ( i=0 ; i<size ; i++ )
	{
		if ( landscape[i][j].height < 0.2*MAXIMUM )
		{
			glVertex3d( (double)j, 0.2*MAXIMUM, (double)i );
		} else {
			glVertex3d( (double)j, landscape[i][j].height, (double)i );
		}
		glVertex3d( (double)j, 0, (double)i );
	}
	glEnd();
		// right edge
	glBegin(GL_TRIANGLE_STRIP);
//	glNormal3f(1,0,0);
	j=size-1;
	for ( i=0 ; i<size ; i++ )
	{
		glVertex3d( (double)j, 0, (double)i );
		if ( landscape[i][j].height < 0.2*MAXIMUM )
		{
			glVertex3d( (double)j, 0.2*MAXIMUM, (double)i );
		} else {
			glVertex3d( (double)j, landscape[i][j].height, (double)i );
		}
	}
	glEnd();
		//top edge
	glBegin(GL_TRIANGLE_STRIP);
//	glNormal3f(0,0,1);
	i=size-1;
	for ( j=0 ; j<size ; j++ )
	{
		if ( landscape[i][j].height < 0.2*MAXIMUM )
		{
			glVertex3d( (double)j, 0.2*MAXIMUM, (double)i );
		} else {
			glVertex3d( (double)j, landscape[i][j].height, (double)i );
		}
		glVertex3d( (double)j, 0, (double)i );
	}
	glEnd();

	// Draw water Plane
	for ( i=0 ; i < size-1 ; i++ )
	{
		// Start drawing a triangle strip
		glBegin(GL_TRIANGLE_STRIP);
		glColor4f(0,0,0.5,0.5);

		for ( j=0 ; j < size ; j++ )
		{
			// first vertex
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, waterSpec);
			glNormal3f(0, 1, 0);
			glVertex3d((double)j, 0.2*MAXIMUM, (double)i);
	
			// second vertex
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, waterSpec);
			glNormal3f(0, 1, 0);
			glVertex3d((double)j, 0.2*MAXIMUM, (double)(i+1));
		}
		glEnd();
	}
}