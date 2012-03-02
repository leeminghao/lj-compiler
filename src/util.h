/**************************************************************************************
 *                                   LJ compiler                                      *
 *                                                                                    *
 * (c) copyright 2011, lmh                                                            *
 * All rights reserved                                                                *
 *                                                                                    *
 * File   : util.h                                                                    *
 * Details: Utility functions for the LJ compiler                                     *
 *                                                                                    *
 * Name   : lmh                                                                       *
 * Date   : 2011.5.21                                                                 *
 * Version: 0.0                                                                       *
 **************************************************************************************/

#ifndef UTIL_H
#define UTIL_H

class Util
{
public:
     Util( void );
    ~Util( void );

    static void      printToken( TokenType, const char * );
    static void      printTree( TreeNode * );
    static TreeNode *newStmtNode( StmtKind );
    static TreeNode *newExpNode( ExpKind );
    static char     *copyString( char * );

private:
    static void printSpaces( void );

    static int s_indentno;
};

#endif // UTIL_H
