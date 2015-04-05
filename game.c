/**************** Header File ****************/
#include "game.h"
#include "landscape.h"

static int CAMERA_FOLLOW = 1;

static double PLAYER1_START_DIRECTION_Y = 0;
static double PLAYER1_START_DIRECTION_Z = 0;

static int lightcycle_move = 0;

/* Global Map */
static map g_Map;

static lightcycle_t *p1;
static lightcycle_t *p2;

/************* INITIALISATION FUNCTIONS *************/

/* Start GLUT, open a window to draw into, etc */
void
InitialiseGLUT(int argc, char **argv)
{	
	// initialize glut
	glutInit(&argc, argv);

	// specify the display for our OpenGL application
	glutInitDisplayMode(GLUT_RGBA				// Use the RGBA colour model
						| GLUT_DOUBLE			// Use double buffering
						| GLUT_DEPTH);			// Use a depth buffer

	// define the size of the window
	glutInitWindowSize(g_WindowWidth, g_WindowHeight);

	// the position where the window will appear
	glutInitWindowPosition(100,100);


	// create the window, giving it a title in the process
	glutCreateWindow(g_ApplicationName);

	// if you want fullscreen, call this function
	//glutFullScreen();

	// what function should be called when the window is resized
	glutReshapeFunc(OnWindowResize);

	// what function should be called when GLUT is idle
	glutIdleFunc(UpdateFrame);
}

/* Do any one-time OpenGL initialisation we might require */
void InitialiseOpenGL()
{
	// define the background, or "clear" colour, for our window
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);

	// Enable Lighting
	glEnable(GL_LIGHTING);

	// Enable Color Material, so I just need to call glColor instead of 2 calls to glMaterial
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);

	// Enable Shading - on by default I think
	glShadeModel(GL_SMOOTH);

	// Enable Blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	// Cull faces to increase speed - won't work yet
	glEnable(GL_CULL_FACE);
}

/************* CALLBACK FUNCTIONS *************/

/* Called whenever the size of the window changes */
void
OnWindowResize(int newWidth, int newHeight)
{
	// save the window width and height for later use
	g_WindowWidth = (float)newWidth;
	g_WindowHeight = (float)newHeight;

	// instruct openGL to use the entirety of our window
	glViewport(0, 0, newWidth, newHeight);

	/* 
	 set up the OpenGL projection matrix, including updated aspect ratio
	 note: "aspect ratio" means "width / height"
	 note: if you wanted your image to "stretch" to fill the window, you would
	       not need to adjust the aspect ratio here.
	 */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FieldOfViewInDegrees, g_WindowWidth / g_WindowHeight, 
				   NearZPlane, FarZPlane);
}

/* Our main update function - called every time we go through our main loop */
void UpdateFrame(void)
{
	/* our timing information */
	unsigned int fps;
	float dt;

	/* force another redraw, so we are always drawing as much as possible */
	glutPostRedisplay();
	
	/* adjust our timer, which we use for transforming our objects */
	dt = GetPreviousFrameDeltaInSeconds();
	g_AnimTimer += dt;
	g_Timer += dt/1000000000;

	if (g_AnimTimer > 1.0f)
	{
		g_AnimTimer = 0.0f;
	}
	
	timeDelta = g_Timer - oldTimer;
	oldTimer = g_Timer;
	/* timing information */
	if (ProcessTimer(&fps))
	{
		/* update our frame rate display */
	//	printf("FPS: %d\n", fps);
	}
}

/************* MAP VIEWING *************/
void SetUpMapView(void)
{
	// Input Functions
	glutKeyboardFunc(OnKeyboardActionMapView);
	glutSpecialFunc(OnSpecialActionMapView);

	BuildMap();

	InitialiseScene();

	glutDisplayFunc(OnDrawSceneMapView);

	/* We want the view to start at the edge of the landscape */
	eyeZ = -g_Size;
}

/* Called whenever a keyboard button is pressed */
void
OnKeyboardActionMapView(unsigned char key, int mousex, int mousey)
{
	/* handle input related to display lists */
	unsigned char keytest = tolower(key);

	/* allow the user to quit with the keyboard */
	if (keytest == 'q')
	{
		exit(0);
	} else if (keytest == '1')
	{
		g_Rotate = ROTATE_OFF;
		// Go to lightcycle game
		StartLightcycle();
	} else if ( keytest == '2' )
	{
		g_Rotate = ROTATE_OFF;
		// Go to snake game
		StartSnake();
	}
	/* Scene Rebuilding */
	else if (keytest == 'r')
	{
		RebuildMap();
	} else
	/* Camera Movement */
	if ( g_Rotate == ROTATE_OFF )
	{
		if ( keytest == 'i' )
		{
			/* to prevent the view going through the centre */
			if ( abs(eyeZ)<=ZOOM_SPEED )
			{
				eyeZ = 0;
			}
			/* Zoom in */
			eyeZ+=ZOOM_SPEED;
		} else if ( keytest == 'o' )
		{
			/* Zoom out */
			eyeZ-=ZOOM_SPEED;
		} else if (keytest == 't')
		{
			// Start Rotating
			g_Rotate = ROTATE_ON;
		}
	} else {
		if ( keytest == 't' )
		{
			// Stop Rotating
			g_Rotate = ROTATE_OFF;
		}
	}
}

/* Keyboard controls for the LightCycle game */
void OnKeyboardActionLightCycle(unsigned char key, int mousex, int mousey)
{
	/* handle input related to display lists */
	unsigned char keytest = tolower(key);

	/* allow the user to quit with the keyboard */
	if (keytest == 'q')
	{
		exit(0);
	}
	/* Camera Movement */
	else if ( keytest == 'i' )
	{
		/* to prevent the view going through the centre */
		if ( abs(eyeZ)<=ZOOM_SPEED )
		{
			eyeZ = 0;
		}
		/* Zoom in */
		eyeZ+=ZOOM_SPEED;
	} else if ( keytest == 'o' )
	{
		/* Zoom out */
		eyeZ-=ZOOM_SPEED;
	}
	else if ( keytest == 'c' )
	{
		if ( CAMERA_FOLLOW == 0 )
		{
			CAMERA_FOLLOW = 1;
		} else if ( CAMERA_FOLLOW == 1 )
		{
			CAMERA_FOLLOW = 0;
		}
	}
	else if ( keytest == 's' )
	{
		if ( p1->isShooting == NOT_SHOOTING )
		{
			Shoot(p1);
		}
	}
	else if ( keytest == 'a' )
	{
		p1->nextTurn = TURN_LEFT;
//		TurnLightCycle(p1, TURN_LEFT);
	}
	else if ( keytest == 'd' )
	{
		p1->nextTurn = TURN_RIGHT;
//		TurnLightCycle(p1, TURN_RIGHT);
	}
}

void
OnSpecialActionLightCycle(unsigned char key, int mousex, int mousey)
{
		if ( key == GLUT_KEY_LEFT )
		{
			p2->nextTurn = TURN_LEFT;
		}else if ( key == GLUT_KEY_RIGHT )
		{
		/* Rotate Camera around y axis  - neg've direction */
			p2->nextTurn = TURN_RIGHT;
		}else if ( key == GLUT_KEY_UP )
		{
		}else if ( key == GLUT_KEY_DOWN )
		{
			if ( p2->isShooting == NOT_SHOOTING )
			{
				Shoot(p2);
			}
		}
}

void
OnSpecialActionMapView(unsigned char key, int mousex, int mousey)
{
	/* Camera Movement */
	if ( g_Rotate == ROTATE_OFF )
	{
		if ( key == GLUT_KEY_LEFT )
		{
			/* Rotate Camera around y axis - pos've direction */
			rotY+=ROTATION_SPEED;
		}else if ( key == GLUT_KEY_RIGHT )
		{
			/* Rotate Camera around y axis  - neg've direction */
			rotY-=ROTATION_SPEED;
		}else if ( key == GLUT_KEY_UP )
		{
		/* Rotate Camera around x axis - pos've direction */
			rotX+=ROTATION_SPEED;
		}else if ( key == GLUT_KEY_DOWN )
		{
			/* Rotate Camera around x axis - neg've direction */
			rotX-=ROTATION_SPEED;
		}
	}
}

void
BuildMap(void)
{
	// Create the Map
	g_Map = generate_map(g_Size);
}

void
RebuildMap(void)
{
	int i;

	/* Free old map */
	for ( i=0; i<g_Size; i++ )
	{
		free(g_Map[i]);
		g_Map[i] = NULL;
	}
	free(g_Map);
	g_Map = NULL;

	// Create a new Map
	BuildMap();

	//Redraw the scene
	InitialiseScene();

}

/* Create all the objects, textures, geometry, etc, that we will use */
void InitialiseScene(void)
{
	/*
	 Create a "display list" containing the landscape and LIGHT0.
	 All the openGL commands that we call in between glNewList and
	 glEndList are saved into the display list with the number
	 g_DisplayList. We can then replay all of these commands by
	 invoking glCallList on this list. Importantly, this will not replay
	 any of the non-OpenGL code (eg, our calls to rand) that was involved
	 in generating the list in this function - so it can be a lot faster
	 */

	/* Create a new list, and store its ID number */
	g_DisplayList = glGenLists(1);

	/* start recording the list */
	glNewList(g_DisplayList, GL_COMPILE);

	/* 
	 fill our list with openGL commands. Here, we will draw the landscape using
	 the plasma fractal algorithm.
	 */
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	/*
	 Lets have the landscape centred around 0,0.
	 Because of how plasma fractal runs, we need this translation
	 */
	glTranslatef(0, MINIMUM+RANGE*1.5, 0);

	InitialiseLight();

	glPopMatrix();

	draw_landscape(g_Map, g_Size);



	/* finish recording the list */
	glEndList();
}

void
InitialiseLight(void)
{
	/* Arrays to specify the properties of LIGHT0 */
	GLfloat ambientLight[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat diffuseLight[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat specularLight[] = { 0.6, 0.6, 0.6, 1.0 };
	GLfloat positionLight[] = { 0.0, 0.0, 0.0, 1.0 };

	/* Array to specify the material properties of the moon object */
	GLfloat moon_colour[] = { 0.6, 0.6, 0.65, 1.0};

	/* Set the properties and enable LIGHT0 */
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, positionLight);
	glEnable(GL_LIGHT0);

	/* Set the material properties for the moon */
	glColor4fv(moon_colour);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, moon_colour);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, moon_colour);

	/* Draw the moon */
	glutSolidSphere(MOON_SIZE, 50, 50);

	/* I don't want anything else to emit light so */
	moon_colour[0] = 0.0;
	moon_colour[1] = 0.0;
	moon_colour[2] = 0.0;
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, moon_colour);
}

/*
 Called whenever the window needs to be redrawn. Although this is technically
 a callback, I have put it here because we will make sure it is called every
 single frame no matter what. As such, it becomes an integral part of our 
 main loop.
 */
void OnDrawSceneMapView(void)
{

	/* clear our colour buffer and depth buffer */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* set up our camera transformation */
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glTranslatef(eyeX, eyeY, eyeZ);

	if ( g_Rotate == ROTATE_ON )
	{
		rotY = 360 * sin(g_Timer*PASSIVE_ROTATION_SPEED);
	}

	glRotatef(rotX, 1.0f, 0.0f, 0.0f);
			/* rotation around x axis */
	glRotatef(rotY, 0.0f, 1.0f, 0.0f);	
			/* rotation around y axis */
	glRotatef(rotZ, 0.0f, 0.0f, 1.0f);
			/* rotation around z axis */

	glTranslatef(-g_Size/2, -MINIMUM-0.5*RANGE, -g_Size/2);

	glCallList(g_DisplayList);

	/*
	 the end of our camera transformations, so we "pop" our camera
	 transformations from the transformation stack
	 */
	glPopMatrix();
	
	/*
	 swap the front and back buffers
	 this achieves what is known as "double buffering"
	 */
	glutSwapBuffers();
}

void 
OnDrawSceneLightCycle(void)
{
	/* clear our colour buffer and depth buffer */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* set up our camera transformation */
	glMatrixMode(GL_MODELVIEW);
	
	glViewport(0, g_WindowHeight / 2, g_WindowWidth, g_WindowHeight / 2);
	
	MoveLightCycle(p1);
	MoveLightCycle(p2);

	glPushMatrix();

	if ( CAMERA_FOLLOW == 1 )
	{
		CameraFollow(p1);
	}
	else
	{
		glTranslatef(eyeX, eyeY, eyeZ);

		glRotatef(rotX, 1.0f, 0.0f, 0.0f);
			/* rotation around x axis */
		glRotatef(rotY, 0.0f, 1.0f, 0.0f);	
			/* rotation around y axis */
		glRotatef(rotZ, 0.0f, 0.0f, 1.0f);
			/* rotation around z axis */
		glTranslatef(-g_Size/2, -MINIMUM-0.5*RANGE, -g_Size/2);
	}

	glPushMatrix();

	glTranslatef(p1->locX, p1->locY, p1->locZ);

	glRotatef(0, 1.0f, 0.0f, 0.0f);
			/* rotation around x axis */
	glRotatef(p1->rotY, 0.0f, 1.0f, 0.0f);	
			/* rotation around y axis */
	glRotatef(0, 0.0f, 0.0f, 1.0f);
			/* rotation around z axis */

	glScalef(LIGHTCYCLE_SIZE_X, LIGHTCYCLE_SIZE_Y, LIGHTCYCLE_SIZE_Z);

	DrawLightCycle(p1);

	glPopMatrix();

	glPushMatrix();

	glTranslatef(p2->locX, p2->locY, p2->locZ);

	glRotatef(0, 1.0f, 0.0f, 0.0f);
			/* rotation around x axis */
	glRotatef(p2->rotY, 0.0f, 1.0f, 0.0f);	
			/* rotation around y axis */
	glRotatef(0, 0.0f, 0.0f, 1.0f);
			/* rotation around z axis */

	glScalef(LIGHTCYCLE_SIZE_X, LIGHTCYCLE_SIZE_Y, LIGHTCYCLE_SIZE_Z);

	DrawLightCycle(p2);

	glPopMatrix();

	glCallList(g_DisplayList);
	DrawTrail(p1);
	DrawTrail(p2);
	DrawProjectile(p1);
	DrawProjectile(p2);

	/*
	 the end of our camera transformations, so we "pop" our camera
	 transformations from the transformation stack
	 */
	glPopMatrix();
	
	glViewport(0, 0, g_WindowWidth, g_WindowHeight / 2);
	
	MoveLightCycle(p1);
	MoveLightCycle(p2);

	glPushMatrix();

	if ( CAMERA_FOLLOW == 1 )
	{
		CameraFollow(p2);
	}
	else
	{
		glTranslatef(eyeX, eyeY, eyeZ);

		glRotatef(rotX, 1.0f, 0.0f, 0.0f);
			/* rotation around x axis */
		glRotatef(rotY, 0.0f, 1.0f, 0.0f);	
			/* rotation around y axis */
		glRotatef(rotZ, 0.0f, 0.0f, 1.0f);
			/* rotation around z axis */
		glTranslatef(-g_Size/2, -MINIMUM-0.5*RANGE, -g_Size/2);
	}

	glPushMatrix();

	glTranslatef(p1->locX, p1->locY, p1->locZ);

	glRotatef(0, 1.0f, 0.0f, 0.0f);
			/* rotation around x axis */
	glRotatef(p1->rotY, 0.0f, 1.0f, 0.0f);	
			/* rotation around y axis */
	glRotatef(0, 0.0f, 0.0f, 1.0f);
			/* rotation around z axis */

	glScalef(LIGHTCYCLE_SIZE_X, LIGHTCYCLE_SIZE_Y, LIGHTCYCLE_SIZE_Z);

	DrawLightCycle(p1);

	glPopMatrix();

	glPushMatrix();

	glTranslatef(p2->locX, p2->locY, p2->locZ);

	glRotatef(0, 1.0f, 0.0f, 0.0f);
			/* rotation around x axis */
	glRotatef(p2->rotY, 0.0f, 1.0f, 0.0f);	
			/* rotation around y axis */
	glRotatef(0, 0.0f, 0.0f, 1.0f);
			/* rotation around z axis */

	glScalef(LIGHTCYCLE_SIZE_X, LIGHTCYCLE_SIZE_Y, LIGHTCYCLE_SIZE_Z);

	DrawLightCycle(p2);

	glPopMatrix();

	glCallList(g_DisplayList);
	DrawTrail(p1);
	DrawTrail(p2);
	DrawProjectile(p1);
	DrawProjectile(p2);

	/*
	 the end of our camera transformations, so we "pop" our camera
	 transformations from the transformation stack
	 */
	glPopMatrix();

	/*
	 swap the front and back buffers
	 this achieves what is known as "double buffering"
	 */
	glutSwapBuffers();

	if ( PLAYER_DEAD != NO_PLAYER )
	{
		PlayerDead(PLAYER_DEAD);
	}
}

void
CameraFollow(lightcycle_t *player)
{
	double camX, camY, camZ;
	double losX, losY, losZ;
	double x, z;
	
	camX = player->locX - player->losX * CAMERA_DISTANCE;
	camZ = player->locZ - player->losZ * CAMERA_DISTANCE;
	if ( camX < 0 )
	{
		x = 0;
	}
	else if ( camX > g_Size - 1 )
	{
		x = g_Size - 1;
	}
	else
	{
		x = camX;
	}
	if ( camZ < 0 )
	{
		z = 0;
	}
	else if ( camZ > g_Size - 1 )
	{
		z = g_Size - 1;
	}
	else
	{
		z = camZ;
	}

	if ( g_Map[(int)z][(int)x].height >= g_Map[(int)player->locZ][(int)player->locX].height )
	{
		camY = g_Map[(int)z][(int)x].height + LIGHTCYCLE_SPEED * timeDelta * heightDelta + CAMERA_DISTANCE;
	}
	else
	{
		camY = player->locY + CAMERA_DISTANCE;
	}

	if ( g_Map[(int)z][(int)x].height <= 0.2 * MAXIMUM )
	{
		camY = player->locY + CAMERA_DISTANCE;
	}
	
	losX = player->locX + player->losX * CAMERA_DISTANCE;
	losZ = player->locZ + player->losZ * CAMERA_DISTANCE;
	losY = player->locY;

	gluLookAt(camX, camY, camZ, losX, losY, losZ, 0, 1, 0);
}

/************* LIGHTCYCLE *************/
void
StartLightcycle(void)
{
	p1 = CreateLightCycle(p1, (int)PLAYER1);
	p2 = CreateLightCycle(p2, (int)PLAYER2);

	InitialiseEdibles();

	// Reassign callback functions
	glutKeyboardFunc(OnKeyboardActionLightCycle);
	glutSpecialFunc(OnSpecialActionLightCycle);
	glutDisplayFunc(OnDrawSceneLightCycle);
	// Create two lightcycles
//	printf("p1->colour[0]: %f\np1->colour[1]: %f\np1->colour[2]: %f\np1->locX: %f\np1->locY: %f\np1->locZ: %f\np1->losX: %f\np1->losY: %f\np1->losZ: %f\np1->rotY: %f\n", p1->colour[0], p1->colour[1], p1->colour[2], p1->locX, p1->locY, p1->locZ, p1->losX, p1->losY, p1->losZ, p1->rotY);
}

/************* SNAKE *************/
void
StartSnake(void)
{
	// Reassign callback functions
/*	glutKeyboardFunc(OnKeyboardActionSnake);
	glutSpecialFunc(OnSpecialActionSnake);
	glutDisplayFunc(OnDrawSceneLightCycle);
*/	// Create two snakes

}

/* Draws a cuboid with width (x1 - x0), height (y1 - y0) and length (z1 - z0) */
void
DrawCuboid(double x0, double x1, double y0, double y1, double z0, double z1)
{
	glBegin(GL_QUADS);
	// front edge (x/y)
	glNormal3d( -1, -1, -1);
	glVertex3d( x0, y0, z0);
	glNormal3d(  1, -1, -1);
	glVertex3d( x1, y0, z0);
	glNormal3d(  1,  1, -1);
	glVertex3d( x1, y1, z0);
	glNormal3d( -1,  1, -1);
	glVertex3d( x0, y1, z0);
	// back edge (x/y)
	glNormal3d( -1, -1, -1);
	glVertex3d( x0, y0, z1);
	glNormal3d(  1, -1, -1);
	glVertex3d( x1, y0, z1);
	glNormal3d(  1,  1, -1);
	glVertex3d( x1, y1, z1);
	glNormal3d( -1,  1, -1);
	glVertex3d( x0, y1, z1);
	// left edge (y/z)
	glNormal3d( -1, -1,  1);
	glVertex3d( x0, y0, z1);
	glNormal3d( -1, -1, -1);
	glVertex3d( x0, y0, z0);
	glNormal3d( -1,  1, -1);
	glVertex3d( x0, y1, z0);
	glNormal3d( -1,  1,  1);
	glVertex3d( x0, y1, z1);
	// right edge (y/z)
	glNormal3d(  1, -1,  1);
	glVertex3d( x1, y0, z0);
	glNormal3d(  1, -1, -1);
	glVertex3d( x1, y0, z1);
	glNormal3d(  1,  1, -1);
	glVertex3d( x1, y1, z1);
	glNormal3d(  1,  1,  1);
	glVertex3d( x1, y1, z0);
	// bottom edge (x/z)
	glNormal3d( -1, -1, -1);
	glVertex3d( x0, y0, z0);
	glNormal3d(  1, -1, -1);
	glVertex3d( x1, y0, z0);
	glNormal3d(  1, -1,  1);
	glVertex3d( x1, y0, z1);
	glNormal3d( -1, -1,  1);
	glVertex3d( x0, y0, z1);
	// top edge (x/z)
	glNormal3d( -1,  1, -1);
	glVertex3d( x0, y1, z0);
	glNormal3d(  1,  1, -1);
	glVertex3d( x1, y1, z0);
	glNormal3d(  1,  1,  1);
	glVertex3d( x1, y1, z1);
	glNormal3d( -1,  1,  1);
	glVertex3d( x0, y1, z1);

	glEnd();
}

/* This function will draw a light cycle, player determines what colour it
  will be:
 * player = PLAYER1 - red
 * player = PLAYER2 - blue */
void
DrawLightCycle(lightcycle_t *player)
{
	// Set the colour for the lightcycle
	glColor4dv(player->colour);

	// Draw the rear wheel
	glPushMatrix();
	glTranslatef( 0, 0.2, -0.3);
	glScalef(1,1,1);
	glutSolidSphere(0.2, 10, 10);
	glPopMatrix();

	// Draw the front wheel
	glPushMatrix();
	glTranslatef( 0, 0.1, 0.4);
	glScalef(1,1,1);
	glutSolidSphere(0.15, 10, 10);
	glPopMatrix();

	// Draw the chasis
	DrawLightcycleChasis();
}

lightcycle_t *
CreateLightCycle(lightcycle_t *player, int player_number)
{
	int count = 0;
	trail_t *trail;
	double *colour;

	/* malloc for light trail and the lightcycle itself */
	trail = malloc(sizeof(trail_t));
	check_pointer(trail, "Failed to Malloc 'trail'");

	colour = malloc(4 * sizeof(double));
	check_pointer(colour, "Failed to Malloc 'colour'");

	player = malloc(sizeof(lightcycle_t));
	check_pointer(player, "Failed to Malloc 'player'");

	player->playerNumber = player_number;

	player->isShooting = NOT_SHOOTING;
	player->nextTurn = NO_TURN;

	/* Set the attributes of the lightcycle depending on the player */
	if ( player_number == PLAYER1 )
	{
		/* Set the initial x and z location of the lightcycle */
		player->locX = PLAYER1_START_LOCATION_X;
		player->locZ = PLAYER1_START_LOCATION_Z;

		player->rotY = 0;

		/* Set the initial direction of the lightcycle, which is the degree of
		 rotation of the lightcycle in the x, y and z direction */
		player->losX = 0; 
		player->losY = 0;
		player->losZ = 1;

		player->bomb = NULL;

		/* Set the colour */
		colour = PLAYER1_COLOUR;
		player->colour = colour;
	}
	else if ( player_number == PLAYER2 )
	{
		/* Set the initial x and z location of the lightcycle */
		player->locX = PLAYER2_START_LOCATION_X;
		player->locZ = PLAYER2_START_LOCATION_Z;

		player->rotY = 0;

		/* Set the initial direction of the lightcycle, which is the degree of
		 rotation of the lightcycle in the x, y and z direction */
		player->losX = 0; 
		player->losY = 0;
		player->losZ = -1;

		player->bomb = NULL;

		/* Set the colour */
		colour = PLAYER2_COLOUR;
		player->colour = colour;
	}

	/* If the lightcycle begins under water, move it forward one unit until it
	  it isn't, or it reaches the middle of the map then try moving it to the 
	  right, then if that doesn't get it out, move it to the left. If all 
	  these fail then the whole map is under water & you should rebuild it */
/*	while ( g_Map[(int)player->locZ][(int)player->locX].height <= 0.2 * MAXIMUM )
	{
		if ( count == g_Size / 2 )
		{
			if ( player->locX == g_Size - 1 )
			{
				player->locX = 0;
			}
			player->locX ++;
			player->locZ = 0;
			count    = 0;
		}
		player->locZ ++;
		count ++;
	}
*/
	/* Set the initial y location of the lightcycle using the heightmap of the
	  terrain */
	player->locY = g_Map[(int)player->locZ][(int)player->locX].height 
		+ LIGHTCYCLE_HOVER_HEIGHT;

	/* Set the location of the light trail to the initial location of the
	  lightcycle */
	trail->locX = player->locX;
	trail->locY = player->locY;
	trail->locZ = player->locZ;
	trail->next = NULL;
	player->trail = trail;
	
	return player;
}

void
MoveLightCycle(lightcycle_t *player)
{
		if ( player->nextTurn != NO_TURN )
		{
			if ( player->losZ == 1 )
			{
				 if ( (int)player->locZ + 1 - player->locZ
					 < LIGHTCYCLE_SPEED * timeDelta )
				 {
					player->locZ = (int)player->locZ + (int)player->losZ;
					TurnLightCycle(player);
				 }
			}
			else if ( player->losZ == -1 )
			{
				if ( player->locZ - (int)player->locZ
					< LIGHTCYCLE_SPEED * timeDelta )
				{
					player->locZ = (int)player->locZ + (int)player->losZ;
					TurnLightCycle(player);
				}
			}
			else if ( player->losX == 1 )
			{
				if ( (int)player->locX + 1 - player->locX
					< LIGHTCYCLE_SPEED * timeDelta )
				{
					player->locX = (int)player->locX + (int)player->losX;
					TurnLightCycle(player);
				}
			}
			else if ( player->losX == -1 )
			{
				if ( player->locX - (int)player->locX 
					< LIGHTCYCLE_SPEED * timeDelta )
				{
					player->locX = (int)player->locX + (int)player->losX;
					TurnLightCycle(player);
				}
			}
		}

		/* Move lightcycle forward, so change the x & z location of lightcycle
		  depending on its direction (using trig) */
		player->locX += LIGHTCYCLE_SPEED * timeDelta * player->losX;
		player->locZ += LIGHTCYCLE_SPEED * timeDelta * player->losZ;

		/* If the lightcycle reaches the edge of the map 
		  or if the lightcycle reaches water */
		if ( player->locX >= g_Size - 1 || player->locX < 1 )
		{
			PLAYER_DEAD = player->playerNumber;
			player->locX -= LIGHTCYCLE_SPEED * timeDelta * player->losX;
		}
		else if ( player->locZ >= g_Size - 1 || player->locZ < 1 )
		{
			PLAYER_DEAD = player->playerNumber;
			player->locZ -= LIGHTCYCLE_SPEED * timeDelta * player->losZ;
		}
		else if ( g_Map[(int)player->locZ][(int)player->locX].height <= 0.2 * MAXIMUM )
		{
			player->locX -= LIGHTCYCLE_SPEED * timeDelta * WATER_SLOW * player->losX;
			player->locZ -= LIGHTCYCLE_SPEED * timeDelta * WATER_SLOW * player->losZ;
		}
		else
		{
/*			heightDelta = g_Map[(int)player->locZ + (int)player->losZ]
				[(int)player->locX + (int)player->losX].height
				- g_Map[(int)player->locZ][(int)player->locX].height;
			player->locY += LIGHTCYCLE_SPEED * timeDelta * heightDelta;
*/
		/* Change the height of the lightcycle depending on the height of the
		  map */
			player->locY = g_Map[(int)player->locZ][(int)player->locX].height + LIGHTCYCLE_HOVER_HEIGHT;
		}
		UpdateTrail(player);
		printf("locX: %.1f\tlocY: %.1f\tlocZ: %.1f\n\n", player->locX, player->locY, player->locZ);
}

void
TurnLightCycle(lightcycle_t *player)
{
		if ( player->nextTurn == TURN_LEFT )
		{
			if ( player->losZ == 0 )
			{
				player->losZ = -player->losX;
				player->losX = 0;
			}
			else if ( player->losX == 0 )
			{
				player->losX = player->losZ;
				player->losZ = 0;
			}
			player->rotY += LIGHTCYCLE_ROTATION_SPEED;
		}
		else if ( player->nextTurn == TURN_RIGHT )
		{
			if ( player->losZ == 0 )
			{
				player->losZ = player->losX;
				player->losX = 0;
			}
			else if ( player->losX == 0 )
			{
				player->losX = -player->losZ;
				player->losZ = 0;
			}
			player->rotY -= LIGHTCYCLE_ROTATION_SPEED;
		}
		if ( rotY >= 360 )
		{
			rotY -= 360;
		}
		else if ( rotY <= -360 )
		{
			rotY += 360;
		}

		player->nextTurn = NO_TURN;
}

/* This function draws the trail for a lightcycle */
void 
DrawTrail(lightcycle_t *player)
{
	int i = 0;
	trail_t *currentTrail, *nextTrail = player->trail;

	// We want both sides of the trail so disable culling
	glDisable(GL_CULL_FACE);

	// Set the colour for the trail
	glColor4f(player->colour[0], player->colour[1], player->colour[2], 0.8);

	// Set the specularity for the trail
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, trailSpecularity);

	// Draw the first segment (from the lightcycle)
/*	DrawTrailSegmentInitial(player->locX, g_Map[(int)player->locZ][(int)player->locX].height, player->locZ,
					nextTrail->locX, nextTrail->locY, nextTrail->locZ);
	*/

	// Keep drawing segments until the end of the trail is reached
	currentTrail = nextTrail;
	nextTrail = nextTrail->next;
	while ( nextTrail != NULL )
	{
		// Set the colour for the trail
		glColor4f(player->colour[0], player->colour[1], player->colour[2], 0.8);

		// Draw the next segment of trail
		DrawTrailSegment( currentTrail->locX, currentTrail->locY, currentTrail->locZ,
					nextTrail->locX, nextTrail->locY, nextTrail->locZ);

		// Get the next trail position
		currentTrail = nextTrail;
		nextTrail = nextTrail->next;

		// Increment the counter
		i++;
	}

	// Enable Culling again
	glEnable(GL_CULL_FACE);

}

/* This function draws the initial trail segment of the trail of a lightcycle */
void
DrawTrailSegmentInitial(double X0, double Y0, double Z0, double X1, double Y1, double Z1)
{
	// Draw a tirangle
	glBegin(GL_TRIANGLES);
	
	glVertex3d(X0, Y0, Z0+TRAIL_HEIGHT/2);

	glVertex3d(X1, Y1, Z1);

	// Set the colour for the upper part of the trail
	glColor4f(1, 1, 1, 0.8);

	glVertex3d(X1, Y1 + TRAIL_HEIGHT, Z1);

	glEnd();
}

/* This function draws a single segment of a trail for a lightcycle */
void
DrawTrailSegment(double X0, double Y0, double Z0, double X1, double Y1, double Z1)
{
	// Draw a quad
	glBegin(GL_QUADS);
	
	glVertex3d(X0, Y0, Z0);

	glVertex3d(X1, Y1, Z1);

	// Set the colour for the upper part of the trail
	glColor4f(1, 1, 1, 0.8);

	glVertex3d(X1, Y1 + TRAIL_HEIGHT, Z1);
	
	glVertex3d(X0, Y0 + TRAIL_HEIGHT, Z0);

	glEnd();
}

void
UpdateTrail(lightcycle_t *player)
{
	double dx;
	double dz;

	while( 1 )
	{
		dx = player->locX - player->trail->locX;
		dz = player->locZ - player->trail->locZ;

		// Do we need to update the trail?
		if ( dx >= 1 )
		{		// Then update the trail
			AddToTrail(player, player->trail->locX+1, player->trail->locZ);
		} else if ( dx <= -1 )
		{		// Then update the trail
			AddToTrail(player, player->trail->locX-1, player->trail->locZ);
		}
		if ( dz >= 1 )
		{		// Then update the trail
			AddToTrail(player, player->trail->locX, player->trail->locZ+1);
		} else if ( dz <= -1)
		{		// Then update the trail
			AddToTrail(player, player->trail->locX, player->trail->locZ-1);
			
		} else {	// Thats done so return
			return;
		}
	}
}

void
AddToTrail(lightcycle_t *player, double x, double z)
{
	double temp;

	trail_t *newTrail = (trail_t *)malloc(sizeof(trail_t));
	check_pointer(newTrail, "Failed to create new trail_t.\n");

	newTrail->locX = x;
	newTrail->locZ = z;

	temp = g_Map[(int)z][(int)x].height;
	if ( temp < 0.2*MAXIMUM )
	{	// We're in the water!
		newTrail->locY = 0.2*MAXIMUM;
	} else {
		newTrail->locY = g_Map[(int)z][(int)x].height;
	}

	CheckTrailCollision(p1->trail, newTrail, player->playerNumber);
	CheckTrailCollision(p2->trail, newTrail, player->playerNumber);

	CheckEdibleCollision(firstEdible, newTrail);

	newTrail->next = player->trail;
	player->trail = newTrail;

	printf("New Trail x: %.1f, y: %.1f, z: %.1f\n", newTrail->locX, newTrail->locY, newTrail->locZ);
}

void
DrawLightcycleChasis()
{
	// Left side
	glBegin(GL_TRIANGLES);
	glVertex3f( -0.075, 0.0, -0.3);
	glVertex3f( -0.075, 0.1, 0.5);
	glVertex3f( -0.075, 0.38, -0.3);

	// Right Side
	glVertex3f( 0.075, 0.38, -0.3);
	glVertex3f( 0.075, 0.1, 0.5);
	glVertex3f( 0.075, 0.0, -0.3);

	glEnd();

	// Top
	glBegin(GL_QUADS);	
	glVertex3f( -0.075, 0.38, -0.3);
	glVertex3f( -0.075, 0.1, 0.5);
	glVertex3f( 0.075, 0.1, 0.5);
	glVertex3f( 0.075, 0.38, -0.3);

	// Bottom
	glVertex3f( -0.075, 0.1, 0.5);
	glVertex3f( -0.075, 0.0, -0.3);
	glVertex3f( 0.075, 0.0, -0.3);
	glVertex3f( 0.075, 0.1, 0.5);

	glEnd();
}

void
Shoot(lightcycle_t *player)
{
	player->isShooting = SHOOTING;

	// Create the bomb
	player->bomb = (projectile *)malloc(sizeof(projectile));
	check_pointer(player->bomb, "Failed to create bomb!\n");

	// Set the variables for the bomb
	player->bomb->originX = player->locX;
	player->bomb->originY = player->locY;
	player->bomb->originZ = player->locZ;

	player->bomb->losX = player->losX;
	player->bomb->losZ = player->losZ;

	player->bomb->initTime = g_Timer;
//	printf("losx:%f,losz:%f\n", player->losX,player->losZ);
//	printf("Created bomb: x:%f, y:%f, z:%f,losX:%f,losZ:%f\n",
//		player->bomb->originX,player->bomb->originY,player->bomb->originZ,player->bomb->losX,player->bomb->losZ);
}

void
DrawProjectile(lightcycle_t *player)
{
	double x, y, z, dt;

	if ( player->bomb != NULL )
	{
		
		// Determine the location of the bomb
		dt = g_Timer - player->bomb->initTime;
		y = -0.5*dt*dt*GRAVITY + dt*INITIAL_VERTICAL_VELOCITY + player->bomb->originY;
		x = player->bomb->losX * PROJECTILE_HORIZONTAL_SPEED * dt + player->bomb->originX;
		z = player->bomb->losZ * PROJECTILE_HORIZONTAL_SPEED * dt + player->bomb->originZ;
//		printf("Drawing bomb: x=%f, y=%f, z=%f, dt:%f\n", x, y, z, dt);

		// Has the bomb left the confines of the map?
		if (  x < 0 || x > g_Size-1 || z < 0 || z > g_Size-1 )
		{
			free(player->bomb);
			player->bomb = NULL;
			player->isShooting = NOT_SHOOTING;
//			printf("left map %f, %f\n", x, z);
		} else {
			// Has the bomb hit the terrain?
			if ( y - g_Map[(int)z][(int)x].height < EXPLOSION_THRESHOLD )
			{
				// Deform the terrain
				DeformTerrain(x,z);

				player->isShooting = NOT_SHOOTING;
//				printf("BOOOOM!!!!!!!! %f, %f\n", x, z);
				// Destroy the bomb
				free(player->bomb);
				player->bomb = NULL;
			}
			else if ( y < 0.2 * MAXIMUM )
			{
				player->isShooting = NOT_SHOOTING;
				free(player->bomb);
				player->bomb = NULL;
			} else {
				// Translate to the correct position
				glPushMatrix();
				glTranslated(x, y, z);

				// Draw the projectile
				glColor4fv(BOMB_COLOUR);
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, BOMB_COLOUR);
				glutSolidSphere(0.6, 10, 10);

				glPopMatrix();
			}
		}
	}
}

void
DeformTerrain(int x, int z)
{
	int r, i, j;

	// Deform the terrain at the given location
	g_Map[z][x].height -= EXPLOSION_STRENGTH;

	// And the surrounding terrain
	for ( r = 1 ; r < EXPLOSION_RADIUS ; r++ )
	{
		for ( i = -r ; i <= r ; i++ )
		{
			for ( j = -r ; j <= r ; j++ )
			{
				if ( z+i < 0 || z+i > g_Size-1 || x+j < 0 || x+j > g_Size-1 )
				{
					// We're outside the map so do nothing
				} else {
					g_Map[z+i][x+j].height -= EXPLOSION_STRENGTH/r;
				}
			}
		}
	}

	// Now Redraw the list
	InitialiseScene();
}

void
PlayerDead(int player_number)
{
	printf("Player %d has died!\n", player_number);

	PLAYER_DEAD = NO_PLAYER;
	FreeLightCycle(p1);
	FreeLightCycle(p2);
	SetUpMapView();
}

void
FreeLightCycle(lightcycle_t *player)
{
	FreeTrail(player->trail);
	free(player);
	player = NULL;
}

void FreeTrail(trail_t *trail)
{
	if (trail->next != NULL)
	{
		FreeTrail(trail->next);
	}
	free(trail);
	trail = NULL;
}

void
DrawEdible(int x, int z)
{
    glPushMatrix();
    glTranslated(x, g_Map[z][x].height+LIGHTCYCLE_HOVER_HEIGHT, z);

    glColor4f(0,1,0,1);
    glutSolidSphere(2, 10, 10);

    glPopMatrix();
}

double
GenerateEdibleCoordinate(void)
{
    return g_Size*(double)rand() / ((double)(RAND_MAX)+(double)(1));
}

void
DrawAllEdibles(edible_t *edible)
{
    if ( edible==NULL )
    {
        // No edible to draw so return;
        return;
    } else {
        // Draw the edible
        DrawEdible(edible->locX, edible->locZ);
        DrawAllEdibles(edible->next);
    }
}

void
AddEdibleToList(edible_t *list)
{
    edible_t *newEdible = (edible_t *)malloc(sizeof(edible_t));

    check_pointer(newEdible,"Failed to malloc edible\n");

    newEdible->locX = GenerateEdibleCoordinate();
    newEdible->locZ = GenerateEdibleCoordinate();
    newEdible->next = NULL;


	if ( list == NULL )
	{
		/* First edible */
		list = newEdible;
	}
	else
	{
		while ( list->next != NULL )
		{
			list = list->next;
		}
	
		// Now we're at the end of the list
	   list->next = newEdible;
	}
}

void
ResetEdible(edible_t *edible)
{
    edible->locX = GenerateEdibleCoordinate();
    edible->locZ = GenerateEdibleCoordinate();
}

void
CheckTrailCollision(trail_t *trail, trail_t *newSegment, int player_number)
{
	if ( trail == NULL )
	{
		return;
	}
	
	else
	{
		if ( newSegment->locX == trail->locX && newSegment->locY == trail->locY && newSegment->locZ == trail->locZ )
		{
			PLAYER_DEAD = player_number;
		}
/*		else if ( p1->locZ == trail->locZ && fabs(p1->locX - trail->locX < 1) )
		{
			PLAYER_DEAD = p1->playerNumber;
		}
*/

		else 
		{
			CheckTrailCollision(trail->next, newSegment, player_number);
		}
	}
}

void
CheckEdibleCollision(edible_t *edible, trail_t *newSegment)
{
	if ( edible == NULL )
	{
		return;
	}
	else
	{
		if ( newSegment->locX == edible->locX && newSegment->locZ == edible->locZ )
		{
			printf("YUM!\n");
			ResetEdible(edible);
		}
		
		CheckEdibleCollision(edible->next, newSegment);
	}
}

void
InitialiseEdibles(void)
{
	AddEdibleToList(firstEdible);
	AddEdibleToList(firstEdible);
	AddEdibleToList(firstEdible);
}