/**************************************************************************************
 *                                   LJ compiler                                      *
 *                                                                                    *
 * (c) copyright 2011, lmh                                                            *
 * All rights reserved                                                                *
 *                                                                                    *
 * File   : parser.h                                                                  *
 * Details: The parser interface for the LJ compiler                                  *
 *                                                                                    *
 * Name   : lmh                                                                       *
 * Date   : 2011.5.01                                                                 *
 * Version: 0.0                                                                       *
 **************************************************************************************/

#ifndef PARSER_H
#define PARSER_H

class Parser
{
public:
     Parser( void );
    ~Parser( void );

    static TreeNode *parse( void );

private:
    static void      syntaxError( const char * );
    static void      match( TokenType );
    static TreeNode *stmtSequence( void );
    static TreeNode *statement( void );
    static TreeNode *ifStmt( void );
    static TreeNode *repeatStmt( void );
    static TreeNode *assignStmt( void );
    static TreeNode *readStmt( void );
    static TreeNode *writeStmt( void );
    static TreeNode *exp( void );
    static TreeNode *simpleExp( void );
    static TreeNode *term( void );
    static TreeNode *factor( void );

    static TokenType s_token;
};

#endif // PARSER_H
