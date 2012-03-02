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

#ifndef CODE_H
#define CODE_H

/* pc = program counter */
#define pc 7

/* mp = "memory pointer" points
 * to top of memory (for temp storage)
 */
#define mp 6

/* gp = "global pointer" points
 * to bottom of memory for (global)
 * variable storage
 */
#define gp 5

/* accumulator */
#define ac 0

/* 2nd accumulator */
#define ac1 1

class Code
{
public:
     Code( void );
    ~Code( void );

    static void emitRO( const char *, int, int, int, const char * );
    static void emitRM( const char *, int, int, int, const char * );
    static int  emitSkip( int );
    static void emitBackup( int );
    static void emitRestore( void );
    static void emitComment( const char * );
    static void emitRMAbs( const char *, int, int, const char * );

private:
    static int s_emitLoc;
    static int s_highEmitLoc;
};

#endif // CODE_H
