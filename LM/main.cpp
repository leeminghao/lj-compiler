/**************************************************************************************
 *                                   LJ compiler                                      *
 *                                                                                    *
 * (c) copyright 2011, lmh                                                            *
 * All rights reserved                                                                *
 *                                                                                    *
 * File   : main.cpp                                                                  *
 * Details: Main program for LJ compiler                                              *
 *                                                                                    *
 * Name   : lmh                                                                       *
 * Date   : 2011.5.27                                                                 *
 * Version: 0.0                                                                       *
 **************************************************************************************/

#include <cstdlib>
#include <cstring>
#include "ljmachine.h"

/* global variable */
FILE *g_code = NULL;

int
main( int argc, char *argv[] )
{
    char  pgm[120];    // source code file name
    bool  done;

    if ( argc != 2 )
    {
	 printf( "usage: %s <filename>\n", argv[0] );
	 exit( 1 );
    }
    strcpy( pgm, argv[1] );
    if ( strchr( pgm, '.' ) == NULL )
    {
	 strcat( pgm, ".lm" );
    }
    g_code = fopen( pgm, "r" );
    if ( g_code == NULL )
    {
	 printf( "file '%s' not found\n", pgm );
	 exit( 1 );
    }
    if ( !LJMachine::readInstructions() )
    {
	 exit( 1 );
    }
    printf( "LM simulation (enter h for help)...\n" );
    do
    {
	 done = !LJMachine::doCommand();
    } while ( !done );
    printf( "Simulation done.\n" );

    return EXIT_SUCCESS;
}
