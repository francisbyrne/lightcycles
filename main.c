/**************** Header File ****************/
#include "main.h"
#include "Timer.h"
#include "game.h"

/**************** Main ****************/
int
main(int argc, char **argv)
{
	/* This may one day work */
	SetRandomSeed();			// sets the a time dependant random seed
	StartupText();				// prints out detail like authors, intructions etc.

	Initialise(argc, argv);		// perform the one time initialisation of GLUT and openGL and anything else
	Start();					// start GLUT
	
	return 0;
}

/**************** FUNCTIONS **************/
void
StartupText(void)
{
	printf("\nWelcome to LightCycles.\nA game by Joshua Torrance & Francis Byrne.\n\n");
	printf("For help, press 'h', otherwise, press enter and get trailing!\n\n");
	if ( getchar() == 'h' )
	{
		printf(" How to play:\n\
Press enter at the menu screen to load the map view.\n\
Map-view controls:\n\
't'     - Stop/Start passive map rotation. Manual map rotation and zooming are\n\
          disabled whilst passive rotation is active.\n\
'w'/'s' - Rotate the map up/down.\n\
'a'/'d' - Rotate the map left/right.\n\
'i'/'o' - Zooms in/out.\n\
'r'     - Rebuild the map.\n\
'1'     - Begins the lightcycle game with the selected map.\n\
'q'     - Quits the program (in both modes).\n\
\n\
In the lightcycle game, player 1 controls the red lightcycle (displayed on the\n\
top screen) while player 2 controls the blue lightcycle (on the bottom).\n\
The lightcycles slow down going across water. Edibles do nothing.\n\
Lightcycle game controls:\n\
'a'/'d'              - Turns the red lightcycle left/right.\n\
's'                  - Shoots a projectile from the red lightcycle.\n\
                       Projectiles make a hole in the terrain.\n\
'left'/'right' arrow - Turns the blue lightcycle left/right.\n\
'down' arrow         - Shoots a projectil from the blue lightcycle.\n\
'c'                  - Switches camera view from behind the lightcycle to\n\
                       mapview with manual map rotation and zooming as per map-\n\
                       view controls.\n\
When a player dies, the game returns to the map-view mode.");
		Wait();
	}
	Wait();
}

/* This function sets a radom time dependant seed */
void
SetRandomSeed(void)
{
	int i, stime;
	long ltime;

	/* get the current calendar time */
	ltime = time(NULL);
	stime = (unsigned) ltime/2;
	srand(stime);
}

void
Initialise(int argc, char **argv)
{
	InitialiseTimer();
	InitialiseGLUT(argc, argv);
	InitialiseOpenGL();
}

void
Start(void)
{
	// Start by viewing the terrain to allow selection of a map.
	SetUpMapView();
	
	// Start GLUT
	glutMainLoop();

}

/* This function simply pauses program flow until a key is preassed */
void
Wait(void)
{
	while ( getchar()==0 )
	{
		// do nothing
	}
}
