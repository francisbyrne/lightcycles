/***************** Libraries *****************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/* GLUT headers */
#include <GL/Glut.h>

/* Maths library - remember to use -lm if building with GCC */
#include <math.h>

/***************** Structures *****************/
/* Lightcycle trail */
typedef struct trail trail_t;
struct trail {
	double locX, locY, locZ;
	trail_t *next;
};

/* Bomb or projectile the lightcycle fires */
typedef struct
{
	double originX, originY, originZ, initTime, losX, losZ;
}projectile;

/* Edibles */
typedef struct edible edible_t;
struct edible {
    int locX, locZ;
    edible_t *next;
};

/* Lightcycle */
typedef struct lightcycle lightcycle_t;
struct lightcycle {
	int playerNumber;
	int isShooting;
	int nextTurn;
	double locX, locY, locZ;
	double losX, losY, losZ;
	double rotY;
	double *colour;
	trail_t *trail;
	projectile *bomb;
};


/***************** Constants *****************/
/* MAP */
#define INITIAL_SIZE_OF_MAP 128
#define MOON_SIZE 15
/* MAP VIEWING */
/* Speed the map rotates when rotating continuously */
#define PASSIVE_ROTATION_SPEED 0.1
/* Amount the map rotates when rotated manually */
#define ROTATION_SPEED 10
/* Amount the map zooms in */
#define ZOOM_SPEED	20
/* Whether continuous map rotation is on or off */
#define ROTATE_ON	1
#define ROTATE_OFF	0

/* The factor that the speed of the lightcycle is reduced by when travelling
  on water (speed = speed - speed * WATER_SLOW)*/
#define WATER_SLOW 0.75

/* Size of the lightcycle */
#define LIGHTCYCLE_SIZE_X 5
#define LIGHTCYCLE_SIZE_Y 5
#define LIGHTCYCLE_SIZE_Z 5

/* Constants for the player number */
#define PLAYER1 1
#define PLAYER2 2
#define NO_PLAYER 0

/* When multiplied by a degree, converts to radians */
#define RAD_CONV 0.0174532925

/* The number of degrees the lightcycle rotates each time a turn is made */
#define LIGHTCYCLE_ROTATION_SPEED 90

/* Turn direction */
#define NO_TURN 0
#define TURN_LEFT 1
#define TURN_RIGHT 2

/* Height of the trail of the lightcycle, go figure */
#define TRAIL_HEIGHT 5

/* Projectile */
#define PROJECTILE_HORIZONTAL_SPEED 2*LIGHTCYCLE_SPEED
#define GRAVITY 100
#define INITIAL_VERTICAL_VELOCITY 1*LIGHTCYCLE_SPEED
#define EXPLOSION_THRESHOLD 0.01
#define EXPLOSION_STRENGTH 5
#define EXPLOSION_RADIUS 10
#define SHOOTING 1
#define NOT_SHOOTING 0

/***************** Globals *****************/
/* Name of the Application */
static const char g_ApplicationName[] = "Tron";

/*
 the width and height of our window - we will
 update these whenever the user resizes the window.
 these could be used, for instance, if you want to 
 position text on the window)
 */
static float g_WindowWidth = 640.0f;
static float g_WindowHeight = 480.0f;

/* our projection settings - we use these in our projection transformation */
static const float NearZPlane = 0.1f;
static const float FarZPlane = 10000.0f;
static const float FieldOfViewInDegrees = 90.0f;

/* our rudimentary animation system */
static float g_Timer = 0.0f;
static float oldTimer = 0.0f;
static float g_AnimTimer = 0.0f;
static float timeDelta = 0.0f;

/* MAP */
/* Map size */
static int g_Size = INITIAL_SIZE_OF_MAP;

/* The id number of our openGL display list which contains the landscape and light source */
static GLuint g_DisplayList = (GLuint)(-1);

/* MAP VIEWING */
/* A flag for continuous map rotation */
static int g_Rotate = ROTATE_ON;
/* Camera position for map viewing */
static float eyeX = 0;
static float eyeY = 0;
static float eyeZ = 0;
/* Rotation of map */
static float rotX = 50;
static float rotY = 0;
static float rotZ = 0;

/* LIGHTCYCLE */
/* A flag for determining whether the lightcycle is turning at the next
  vertex*/
static int nextTurn = NO_TURN;

/* The height at which the lightcycle hovers off the ground */
static double LIGHTCYCLE_HOVER_HEIGHT = 5;

/* The difference in height between the vertex the lightcycle was at
  and the vertex the lightcycle will be at next */
static double heightDelta = 0.0f;

/* The angle that directs the lightcycle's line of sight */
static double LIGHTCYCLE_TURN_ANGLE = 0;

/* The amount of units the lightcycle travels with each forward movement */
static int LIGHTCYCLE_SPEED = 3;

/* The distance the camera is from the player during the game */
static double CAMERA_DISTANCE = 2 * (LIGHTCYCLE_SIZE_X + LIGHTCYCLE_SIZE_Y + LIGHTCYCLE_SIZE_Z) / 3;

/* The start location of the lightcycle on the x-z plane */
static double PLAYER1_START_LOCATION_X = INITIAL_SIZE_OF_MAP / 2;
static double PLAYER1_START_LOCATION_Z = 1;
static double PLAYER2_START_LOCATION_X = INITIAL_SIZE_OF_MAP / 2;
static double PLAYER2_START_LOCATION_Z = INITIAL_SIZE_OF_MAP - 2;

/* Specularity arrays */
static GLfloat trailSpecularity[4] = { 0.4, 0.4, 0.4, 0.3 };
static GLfloat lightcycleSpecularity[4] = { 0.2, 0.2, 0.2, 1 };

/* Flag for checking whether a player has died */
static int PLAYER_DEAD = NO_PLAYER;

/* Player colours */
static double PLAYER1_COLOUR[4] = {1, 0, 0, 1}; /* Red */
static double PLAYER2_COLOUR[4] = {0, 0, 1, 1}; /* Blue */

/* Projectiles */
static GLfloat BOMB_COLOUR[4] = {0.3, 0.3, 0.3, 1}; /* grey */

/* Edibles */
static edible_t *firstEdible;

/***************** Function Prototypes *****************/
// Initialisation
void InitialiseGLUT(int argc, char **argv);
void InitialiseOpenGL();
// Call Back
void OnWindowResize(int newWidth, int newHeight);
void UpdateFrame(void);
// Map Viewing
void SetUpMapView(void);
void OnKeyboardActionMapView(unsigned char key, int mousex, int mousey);
void OnSpecialActionMapView(unsigned char key, int mousex, int mousey);
void BuildMap(void);
void RebuildMap(void);
void InitialiseLight(void);
void InitialiseScene(void);
void OnDrawSceneMapView(void);
/* Lightcycle */
void StartLightcycle(void);
lightcycle_t *CreateLightCycle(lightcycle_t *player, int player_number);
void CameraFollow(lightcycle_t *player);
void OnDrawSceneLightCycle(void);
void DrawLightCycle(lightcycle_t *player);
void DrawLightcycleChasis(void);
void DrawCuboid(double x0, double x1, double y0, double y1, double z0, double z1);
void MoveLightCycle(lightcycle_t *player);
void TurnLightCycle(lightcycle_t *player);
/* Lightcycle Trail */
void DrawTrail(lightcycle_t *player);
void DrawTrailSegment(double, double, double, double, double, double);
void UpdateTrail(lightcycle_t *player);
void AddToTrail(lightcycle_t *player, double x, double z);
/* Shooting */
void Shoot(lightcycle_t *player);
void DrawProjectile(lightcycle_t *player);
void DeformTerrain(int x, int z);
/* End of Game */
void PlayerDead(int player_number);
void FreeLightCycle(lightcycle_t *player);
void FreeTrail(trail_t *trail);
/* Edibles */
void DrawEdible(int x, int z);
double GenerateEdibleCoordinate(void);
void DrawAllEdibles(edible_t *edible);
void AddEdibleToList(edible_t *list);
void ResetEdible(edible_t *edible);
void InitialiseEdibles(void);
/* Collision */
void CheckTrailCollision(trail_t *trail, trail_t *newSegment, int player_number);
void CheckEdibleCollision(edible_t *edible, trail_t *newSegment);

//Snake
void StartSnake(void);

/***************** Error Messages *****************/
