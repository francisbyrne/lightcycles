/**
 * utility.c
 * Useful general functions.
 * Created in 2008 for 433-380 Project 2.
 * Written by Josh Torrance with additions by Francis Byrne.
 */

/**************** Header File ****************/
#include "utility.h"

/**************** Functions ****************/

/* This function checks to see if a pointer is NULL
 * If it is then an error message is printed and the program exits */
void
check_pointer(void *pointer, char *error_message)
{
	if ( pointer == NULL )
	{
		fprintf(stderr,error_message);
		exit(EXIT_FAILURE);
	}
}