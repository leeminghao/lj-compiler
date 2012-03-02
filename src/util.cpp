/**************************************************************************************
 *                                   LJ compiler                                      *
 *                                                                                    *
 * (c) copyright 2011, lmh                                                            *
 * All rights reserved                                                                *
 *                                                                                    *
 * File   : util.cpp                                                                  *
 * Details: Utility functions implementation for the LJ compiler                      *
 *                                                                                    *
 * Name   : lmh                                                                       *
 * Date   : 2011.5.21                                                                 *
 * Version: 0.0                                                                       *
 **************************************************************************************/

#include "globals.h"
#include "util.h"

Util::Util( void )
{
}

Util::~Util( void )
{
}

int Util::s_indentno = 0;

/* Procedure PrintToken prints a token
 * and its lexeme to the listing file
 */
void
Util::printToken( TokenType token, const char *tokenString )
{
    switch ( token )
    {
        case IF:
        case THEN:
        case ELSE:
        case END:
        case REPEAT:
        case UNTIL:
        case READ:
        case WRITE:
            fprintf( g_listing, "reserved word: %s\n", tokenString );
            break;

        case ASSIGN:
            fprintf( g_listing, ":=\n" );
            break;
        case LT:
            fprintf( g_listing, "<\n" );
            break;
        case EQ:
            fprintf( g_listing, "=\n" );
            break;
        case LPAREN:
            fprintf( g_listing, "(\n" );
            break;
        case RPAREN:
            fprintf( g_listing, ")\n" );
            break;
        case SEMI:
            fprintf( g_listing, ";\n" );
            break;
        case PLUS:
            fprintf( g_listing, "+\n" );
            break;
        case MINUS:
            fprintf( g_listing, "-\n" );
            break;
        case TIMES:
            fprintf( g_listing, "*\n" );
            break;
        case OVER:
            fprintf( g_listing, "/\n" );
            break;

        case NUM:
            fprintf( g_listing, "NUM, val = %s\n", tokenString );
            break;
        case ID:
            fprintf( g_listing, "ID, name = %s\n", tokenString );
            break;

        case ENDFILE:
            fprintf( g_listing, "EOF\n" );
            break;
        case ERROR:
            fprintf( g_listing, "ERROR: %s\n", tokenString );
            break;

        default:
            fprintf( g_listing, "Unknown token: %d\n", token );
            break;
    }
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode *
Util::newStmtNode( StmtKind kind )
{
    TreeNode *t = new TreeNode;

    if ( t == NULL )
    {
        fprintf( g_listing, "Out of memory error at line %d\n", g_lineno );
    }
    else
    {
        for ( int i = 0; i < MAXCHILDREN; i++ )
        {
            t->m_child[i] = NULL;
        }
        t->m_sibling     = NULL;
        t->m_nodekind    = StmtK;
        t->m_kind.u_stmt = kind;
        t->m_lineno      = g_lineno;
    }

    return t;
}

/* Function newExpNode creates a new expression
 * node for syntax tree construction
 */
TreeNode *
Util::newExpNode( ExpKind kind )
{
    TreeNode *t = new TreeNode;

    if ( t == NULL )
    {
        fprintf( g_listing, "Out of memory error at line %d\n", g_lineno );
    }
    else
    {
        for ( int i = 0; i < MAXCHILDREN; i++ )
        {
            t->m_child[i] = NULL;
        }
        t->m_sibling    = NULL;
        t->m_nodekind   = ExpK;
        t->m_kind.u_exp = kind;
        t->m_lineno     = g_lineno;
        t->m_type       = Void;
    }

    return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char *
Util::copyString( char *s )
{
    int   n;
    char *t;

    if ( s == NULL )
    {
        return NULL;
    }
    n = strlen( s ) + 1;
    t = new char[n];
    if ( t == NULL )
    {
        fprintf( g_listing, "Out of memory error at line %d\n", g_lineno );
    }
    else
    {
        strcpy( t, s );
    }

    return t;
}

/* printSpaces indents by printing spaces */
void
Util::printSpaces( void )
{
    for ( int i = 0; i < s_indentno; i++ )
    {
        fprintf( g_listing, " " );
    }
}

/* procedure printTree prints a syntax tree to the
 * g_listing file using indentation to indicate subtrees
 */
void
Util::printTree( TreeNode *tree )
{
    s_indentno += 2;
    while ( tree != NULL )
    {
        printSpaces();
        if ( tree->m_nodekind == StmtK )
        {
            switch ( tree->m_kind.u_stmt )
            {
                case IfK:
                    fprintf( g_listing, "If\n" );
                    break;

                case RepeatK:
                    fprintf( g_listing, "Repeat\n" );
                    break;

                case AssignK:
                    fprintf( g_listing, "Assign to: %s\n", tree->m_attr.u_name );
                    break;

                case ReadK:
                    fprintf( g_listing, "Read: %s\n", tree->m_attr.u_name );
                    break;

                case WriteK:
                    fprintf( g_listing, "Write\n" );
                    break;

                default:
                    fprintf( g_listing, "Unknown StmtNode kind\n" );
                    break;
            }
        }
        else if ( tree->m_nodekind == ExpK )
        {
            switch ( tree->m_kind.u_exp )
            {
                case OpK:
                    fprintf( g_listing, "Op: " );
                    printToken( tree->m_attr.u_op, "\0" );
                    break;

                case ConstK:
                    fprintf( g_listing, "Const: %d\n", tree->m_attr.u_val );
                    break;

                case IdK:
                    fprintf( g_listing, "Id: %s\n", tree->m_attr.u_name );
                    break;

                default:
                    fprintf( g_listing, "Unknown ExpNode kind\n" );
                    break;
            }
        }
        else
        {
            fprintf( g_listing, "Unknown node kind\n" );
        }

        for ( int i = 0; i < MAXCHILDREN; i++ )
        {
            printTree( tree->m_child[i] );
        }
        tree = tree->m_sibling;
    }
    s_indentno -= 2;
}
