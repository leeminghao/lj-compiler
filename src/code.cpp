/**************************************************************************************
 *                                   LJ compiler                                      *
 *                                                                                    *
 * (c) copyright 2011, lmh                                                            *
 * All rights reserved                                                                *
 *                                                                                    *
 * File   : code.h                                                                    *
 * Details: Code emitting utilities interface for the LJ compiler                     *
 *                                                                                    *
 * Name   : lmh                                                                       *
 * Date   : 2011.5.17                                                                 *
 * Version: 0.0                                                                       *
 **************************************************************************************/

#include "globals.h"
#include "code.h"

Code::Code( void )
{
}

Code::~Code( void )
{
}

/* TM location number for current instruction emission */
int Code::s_emitLoc = 0;

/* Highest TM location emitted so far
 * For use in conjunction with emitSkip,
 * emitBackup, and emitRestore
 */
int Code::s_highEmitLoc = 0;

/* Procedure emitComment prints a comment line
 * with comment c in the code file
 */
void
Code::emitComment( const char *c )
{
    if ( g_traceCode )
    {
        fprintf( g_code, "* %s\n", c );
    }
}

/* Procedure emitRO emits a register-only
 * TM instruction
 * op = the opcode
 * r  = target register
 * s  = 1st source register
 * t  = 2nd source register
 * c  = a comment to be printed if g_traceCode is true
 */
void
Code::emitRO( const char *op, int r, int s, int t, const char *c )
{
    fprintf( g_code, "%3d:    %5s    %d,%d,%d ", s_emitLoc++, op, r, s, t );
    if ( g_traceCode )
    {
        fprintf( g_code, "\t%s", c );
    }
    fprintf( g_code, "\n" );
    if ( s_highEmitLoc < s_emitLoc )
    {
        s_highEmitLoc = s_emitLoc;
    }
}

/* Procedure emitRM emits a register-to-memory
 * TM instruction
 * op = the opcode
 * r  = target register
 * d  = the offset
 * s  = the base register
 * c  = a comment to be printed if g_traceCode is true
 */
void
Code::emitRM( const char *op, int r, int d, int s, const char *c )
{
    fprintf( g_code, "%3d:    %5s    %d,%d(%d) ", s_emitLoc++, op, r, d, s );
    if ( g_traceCode )
    {
        fprintf( g_code, "\t%s", c );
    }
    fprintf( g_code, "\n" );
    if ( s_highEmitLoc < s_emitLoc )
    {
        s_highEmitLoc = s_emitLoc;
    }
}

/*
 * Function emitSkip skips "howMany" code
 * locations for later backpatch. It also
 * returns the current code position
 */
int
Code::emitSkip( int howMany )
{
    int i = s_emitLoc;

    s_emitLoc += howMany;
    if ( s_highEmitLoc < s_emitLoc )
    {
        s_highEmitLoc = s_emitLoc;
    }

    return i;
}

/*
 * Procedure emitBackup backs up to
 * loc = a previously skipped location
 */
void
Code::emitBackup( int loc )
{
    if ( loc > s_highEmitLoc )
    {
        emitComment( "BUG in emitBackup" );
    }
    s_emitLoc = loc;
}

/* Procedure emitRestore restores the current
 * code position to the highest previously
 * unemitted position
 */
void
Code::emitRestore( void )
{
    s_emitLoc = s_highEmitLoc;
}

/* Procedure emitRM_Abs converts an absolute reference
 * to a pc-relative reference when emitting a
 * register-to-memory TM instruction
 * op = the opcode
 * r  = target register
 * a  = teh absolute location in memory
 * c  = a comment to be printed if g_traceCode is true
 */
void
Code::emitRMAbs( const char *op, int r, int a, const char *c )
{
    fprintf( g_code, "%3d:    %5s    %d,%d(%d) ",
        s_emitLoc, op, r, a - (s_emitLoc + 1), pc );
    ++s_emitLoc;
    if ( g_traceCode )
    {
        fprintf( g_code, "\t%s", c );
    }
    fprintf( g_code, "\n" );
    if ( s_highEmitLoc < s_emitLoc )
    {
        s_highEmitLoc = s_emitLoc;
    }
}
