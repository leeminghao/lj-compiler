/**************************************************************************************
 *                                   LJ compiler                                      *
 *                                                                                    *
 * (c) copyright 2011, lmh                                                            *
 * All rights reserved                                                                *
 *                                                                                    *
 * File   : analyze.h                                                                 *
 * Details: Semantic analyzer interface for the LJ compiler                           *
 *                                                                                    *
 * Name   : lmh                                                                       *
 * Date   : 2011.5.13                                                                 *
 * Version: 0.0                                                                       *
 **************************************************************************************/

#ifndef ANALYZE_H
#define ANALYZE_H

class Analyze
{
public:
     Analyze( void );
    ~Analyze( void );

    static void buildSymtab( TreeNode * );
    static void typeCheck( TreeNode * );

private:
    static void traverse( TreeNode *, void (*)( TreeNode* ), void (*)( TreeNode* ) );
    static void nullProc( TreeNode * );
    static void insertNode( TreeNode * );
    static void typeError( TreeNode *, const char * );
    static void checkNode( TreeNode * );

    static int s_location;
};

#endif // ANALYZE_H
