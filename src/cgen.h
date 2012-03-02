/**************************************************************************************
 *                                   LJ compiler                                      *
 *                                                                                    *
 * (c) copyright 2011, lmh                                                            *
 * All rights reserved                                                                *
 *                                                                                    *
 * File   : cgen.h                                                                    *
 * Details: The code generator interface for the LJ compiler                          *
 *                                                                                    *
 * Name   : lmh                                                                       *
 * Date   : 2011.5.23                                                                 *
 * Version: 0.0                                                                       *
 **************************************************************************************/

#ifndef CGEN_H
#define CGEN_H

class CodeGenerator
{
public:
     CodeGenerator( void );
    ~CodeGenerator( void );

    static void codeGen( TreeNode *, char * );

private:
    static void cGen( TreeNode * );
    static void genStmt( TreeNode * );
    static void genExp( TreeNode * );

    static int s_tmpOffset;
};

#endif // CGEN_H
