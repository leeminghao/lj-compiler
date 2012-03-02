/**************************************************************************************
 *                                   LJ compiler                                      *
 *                                                                                    *
 * (c) copyright 2011, lmh                                                            *
 * All rights reserved                                                                *
 *                                                                                    *
 * File   : parser.cpp                                                                *
 * Details: The parser implementation for the LJ compiler                             *
 *                                                                                    *
 * Name   : lmh                                                                       *
 * Date   : 2011.5.02                                                                 *
 * Version: 0.0                                                                       *
 **************************************************************************************/

#include "globals.h"
#include "util.h"
#include "lexer.h"
#include "parser.h"

Parser::Parser( void )
{
}

Parser::~Parser( void )
{
}

TokenType Parser::s_token;

void
Parser::syntaxError( const char *message )
{
    fprintf( g_listing, "\n>>> " );
    fprintf( g_listing, "Syntax error at line %d: %s", g_lineno, message );
    g_error = true;
}

void
Parser::match( TokenType expected )
{
    if ( s_token == expected )
    {
        s_token = Lexer::getToken();
    }
    else
    {
        syntaxError( "unexpected token -> " );
        Util::printToken( s_token, g_tokenString );
        fprintf( g_listing, "    " );
    }
}

TreeNode *
Parser::stmtSequence( void )
{
    TreeNode *t = statement();
    TreeNode *p = t;

    while ( (s_token != ENDFILE) && (s_token != END) &&
            (s_token != ELSE)    && (s_token != UNTIL) )
    {
        TreeNode *q;

        match( SEMI );
        q = statement();
        if ( q != NULL )
        {
            if ( t == NULL )
            {
                t = p = q;
            }
            else    // now p connot be NULL either
            {
                p->m_sibling = q;
                p            = q;
            }
        }
    }

    return t;
}

TreeNode *
Parser::statement( void )
{
    TreeNode *t = NULL;

    switch ( s_token )
    {
        case IF:
            t = ifStmt();
            break;

        case REPEAT:
            t = repeatStmt();
            break;

        case ID:
            t = assignStmt();
            break;

        case READ:
            t = readStmt();
            break;

        case WRITE:
            t = writeStmt();
            break;

        default:
            syntaxError( "unexpected token -> " );
            Util::printToken( s_token, g_tokenString );
            s_token = Lexer::getToken();
            break;
    }

    return t;
}

TreeNode *
Parser::ifStmt( void )
{
    TreeNode *t = Util::newStmtNode( IfK );

    match( IF );
    if ( t != NULL )
    {
        t->m_child[0] = exp();
    }
    match( THEN );
    if ( t != NULL )
    {
        t->m_child[1] = stmtSequence();
    }
    if ( s_token == ELSE )
    {
        match( ELSE );
        if ( t != NULL )
        {
            t->m_child[2] = stmtSequence();
        }
    }
    match( END );

    return t;
}

TreeNode *
Parser::repeatStmt( void )
{
    TreeNode *t = Util::newStmtNode( RepeatK );

    match( REPEAT );
    if ( t != NULL )
    {
        t->m_child[0] = stmtSequence();
    }
    match( UNTIL );
    if ( t != NULL )
    {
        t->m_child[1] = exp();
    }

    return t;
}

TreeNode *
Parser::assignStmt( void )
{
    TreeNode *t = Util::newStmtNode( AssignK );

    if ( (t != NULL) && (s_token == ID) )
    {
        t->m_attr.u_name = Util::copyString( g_tokenString );
    }
    match( ID );
    match( ASSIGN );
    if ( t != NULL )
    {
        t->m_child[0] = exp();
    }

    return t;
}

TreeNode *
Parser::readStmt( void )
{
    TreeNode *t = Util::newStmtNode( ReadK );

    match( READ );
    if ( (t != NULL) && (s_token == ID) )
    {
        t->m_attr.u_name = Util::copyString( g_tokenString );
    }
    match( ID );

    return t;
}

TreeNode *
Parser::writeStmt( void )
{
    TreeNode *t = Util::newStmtNode( WriteK );

    match( WRITE );
    if ( t != NULL )
    {
        t->m_child[0] = exp();
    }

    return t;
}

TreeNode *
Parser::exp( void )
{
    TreeNode *t = simpleExp();

    if ( (s_token == LT) || (s_token == EQ) )
    {
        TreeNode *p = Util::newExpNode( OpK );

        if ( p != NULL )
        {
            p->m_child[0]  = t;
            p->m_attr.u_op = s_token;
            t              = p;
        }
        match( s_token );
        if ( t != NULL )
        {
            t->m_child[1] = simpleExp();
        }
    }

    return t;
}

TreeNode *
Parser::simpleExp( void )
{
    TreeNode *t = term();

    while ( (s_token == PLUS) || (s_token == MINUS) )
    {
        TreeNode *p = Util::newExpNode( OpK );

        if ( p != NULL )
        {
            p->m_child[0]  = t;
            p->m_attr.u_op = s_token;
            t              = p;
            match( s_token );
            t->m_child[1]  = term();
        }
    }

    return t;
}

TreeNode *
Parser::term( void )
{
    TreeNode *t = factor();

    while ( (s_token == TIMES) || (s_token == OVER) )
    {
        TreeNode *p = Util::newExpNode( OpK );

        if ( p != NULL )
        {
            p->m_child[0]  = t;
            p->m_attr.u_op = s_token;
            t              = p;
            match( s_token );
            p->m_child[1]  = factor();
        }
    }

    return t;
}

TreeNode *
Parser::factor( void )
{
    TreeNode *t = NULL;

    switch ( s_token )
    {
        case NUM:
            t = Util::newExpNode( ConstK );
            if ( (t != NULL) && (s_token == NUM) )
            {
                t->m_attr.u_val = atoi( g_tokenString );
            }
            match( NUM );
            break;

        case ID:
            t = Util::newExpNode( IdK );
            if ( (t != NULL) && (s_token == ID) )
            {
                t->m_attr.u_name = Util::copyString( g_tokenString );
            }
            match( ID );
            break;

        case LPAREN:
            match( LPAREN );
            t = exp();
            match( RPAREN );
            break;

        default:
            syntaxError( "unexpected token -> " );
            Util::printToken( s_token, g_tokenString );
            s_token = Lexer::getToken();
            break;
    }

    return t;
}

/* Function parse returns the newly
 * constructed syntax tree
 */
TreeNode *
Parser::parse( void )
{
    TreeNode *t;

    s_token = Lexer::getToken();
    t       = stmtSequence();
    if ( s_token != ENDFILE )
    {
        syntaxError( "Code ends before file\n" );
    }

    return t;
}
