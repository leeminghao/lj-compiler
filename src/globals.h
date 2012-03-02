/**************************************************************************************
 *                                   LJ compiler                                      *
 *                                                                                    *
 * (c) copyright 2011, lmh                                                            *
 * All rights reserved                                                                *
 *                                                                                    *
 * File   : globals.h                                                                 *
 * Details: Global types and vars for LJ compiler                                     *
 *          must come before other include files                                      *
 *                                                                                    *
 * Name   : lmh                                                                       *
 * Date   : 2011.5.21                                                                 *
 * Version: 0.0                                                                       *
 **************************************************************************************/

#ifndef GLOBALS_H
#define GLOBALS_H

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstring>

/* MAXRESERVED = the number of reserved words */
#define MAXRESERVED 8

typedef enum
{
    /* book-keeping tokens */
    ENDFILE, ERROR,
    /* reserved words */
    IF, THEN, ELSE, END, REPEAT, UNTIL, READ, WRITE,
    /* multicharacter tokens */
    ID, NUM,
    /* special symbols */
    ASSIGN, EQ, LT, PLUS, MINUS, TIMES, OVER, LPAREN, RPAREN, SEMI

} TokenType;

extern FILE *g_source;    // source code text file
extern FILE *g_listing;   // listing ouput text file
extern FILE *g_code;      // code text file for TM simulator

extern int   g_lineno;    // source line number for listing

/**************************************************************************************
 *                              Syntax tree for parsing                               *
 **************************************************************************************/

typedef enum { StmtK, ExpK } NodeKind;
typedef enum { IfK, RepeatK, AssignK, ReadK, WriteK } StmtKind;
typedef enum { OpK, ConstK, IdK } ExpKind;

/* ExpType is used for type checking */
typedef enum { Void, Integer, Boolean } ExpType;

#define MAXCHILDREN 3

typedef struct treeNode
{
    struct treeNode *m_child[MAXCHILDREN];
    struct treeNode *m_sibling;
    int              m_lineno;
    NodeKind         m_nodekind;
    union
    {
        StmtKind u_stmt;
        ExpKind  u_exp;
    } m_kind;
    union
    {
        TokenType u_op;
        int       u_val;
        char     *u_name;
    } m_attr;
    ExpType          m_type;    // for type checking of exps

} TreeNode;

/**************************************************************************************
 *                                Flags for tracing                                   *
 **************************************************************************************/

/* g_echoSource = true causes the source program to
 * be echoed to the listing file with line numbers
 * during parsing
 */
extern bool g_echoSource;

/* g_traceScan = true causes token information to be
 * printed to the listing file as each token is
 * recognized by the lexer
 */
extern bool g_traceScan;

/* g_traceParse = true causes the syntax tree to be
 * printed to the listing file in linearized form
 * (using indents for children)
 */
extern bool g_traceParse;

/* g_traceAnalyze = true causes symbol table inserts
 * and lookups to be reported to the listing file
 */
extern bool g_traceAnalyze;

/* g_traceCode = true causes comments to be written
 * to the TM code file as code is generate
 */
extern bool g_traceCode;

/* g_error = true prevents further passes if an error occurs */
extern bool g_error;

#endif // GLOBALS_H
