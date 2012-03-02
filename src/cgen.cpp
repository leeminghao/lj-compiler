/**************************************************************************************
 *                                   LJ compiler                                      *
 *                                                                                    *
 * (c) copyright 2011, lmh                                                            *
 * All rights reserved                                                                *
 *                                                                                    *
 * File   : cgen.cpp                                                                  *
 * Details: The code generator impementation for the LJ compiler                      *
 *                                                                                    *
 * Name   : lmh                                                                       *
 * Date   : 2011.5.23                                                                 *
 * Version: 0.0                                                                       *
 **************************************************************************************/

#include "globals.h"
#include "symtab.h"
#include "code.h"
#include "cgen.h"

CodeGenerator::CodeGenerator( void )
{
}

CodeGenerator::~CodeGenerator( void )
{
}

/* s_tmpOffset is the memory offset for temps
 * It is decremented each time a temp is
 * stored, and incremented when loaded again
 */
int CodeGenerator::s_tmpOffset = 0;

/* Procedure genStmt generates code at a statement node */
void
CodeGenerator::genStmt( TreeNode *tree )
{
    TreeNode *p1;
    TreeNode *p2;
    TreeNode *p3;
    int       savedLoc1;
    int       savedLoc2;
    int       currentLoc;
    int       loc;

    switch ( tree->m_kind.u_stmt )
    {
        case IfK:
            if ( g_traceCode )
            {
                Code::emitComment( "-> if" );
            }
            p1 = tree->m_child[0];
            p2 = tree->m_child[1];
            p3 = tree->m_child[2];
            /* generate code for test expression */
            cGen( p1 );
            savedLoc1 = Code::emitSkip( 1 );
            Code::emitComment( "if: jump to else belongs here" );
            /* recurse on then part */
            cGen( p2 );
            savedLoc2 = Code::emitSkip( 1 );
            Code::emitComment( "if: jump to end belongs here" );
            currentLoc = Code::emitSkip( 0 );
            Code::emitBackup( savedLoc1 );
            Code::emitRMAbs( "JEQ", ac, currentLoc, "if: jmp to else" );
            Code::emitRestore();
            /* recurse on else part */
            cGen( p3 );
            currentLoc = Code::emitSkip( 0 );
            Code::emitBackup( savedLoc2 );
            Code::emitRMAbs( "LDA", pc, currentLoc, "jmp to end" );
            Code::emitRestore();
            if ( g_traceCode )
            {
               Code::emitComment( "<- if" );
            }
            break;

        case RepeatK:
            if ( g_traceCode )
            {
                Code::emitComment( "-> repeat" );
            }
            p1 = tree->m_child[0];
            p2 = tree->m_child[1];
            savedLoc1 = Code::emitSkip( 0 );
            Code::emitComment( "repeat: jump after body comes back here" );
            /* generate code for body */
            cGen( p1 );
            /* generate code for test */
            cGen( p2 );
            Code::emitRMAbs( "JEQ", ac, savedLoc1, "repeat: jmp back to body" );
            if ( g_traceCode )
            {
                Code::emitComment( "<- repeat" );
            }
            break;

        case AssignK:
            if ( g_traceCode )
            {
                Code::emitComment( "-> assign" );
            }
            /* generate code for rhs */
            cGen( tree->m_child[0] );
            /* now store value */
            loc = SymTab::lookup( tree->m_attr.u_name );
            Code::emitRM( "ST", ac, loc, gp, "assign: store value" );
            if ( g_traceCode )
            {
                Code::emitComment( "<- assign" );
            }
            break;

        case ReadK:
            Code::emitRO( "IN", ac, 0, 0, "read integer value" );
            loc = SymTab::lookup( tree->m_attr.u_name );
            Code::emitRM( "ST", ac, loc, gp, "read: store value" );
            break;

        case WriteK:
            /* generate code for expression to write */
            cGen( tree->m_child[0] );
            /* now output it */
            Code::emitRO( "OUT", ac, 0, 0, "write ac" );
            break;

        default:
            break;
    }
}

/* Procudure genExp generates code at an expression node */
void
CodeGenerator::genExp( TreeNode *tree )
{
    int       loc;
    TreeNode *p1, *p2;

    switch ( tree->m_kind.u_exp )
    {
        case ConstK:
            if ( g_traceCode )
            {
                Code::emitComment( "-> Const" );
            }
            /* gen code to load integer constant using LDC */
            Code::emitRM( "LDC", ac, tree->m_attr.u_val, 0, "load cosnt" );
            if ( g_traceCode )
            {
                Code::emitComment( "<- Const" );
            }
            break;

        case IdK:
            if ( g_traceCode )
            {
                Code::emitComment( "-> Id" );
            }
            loc = SymTab::lookup( tree->m_attr.u_name );
            Code::emitRM( "LD", ac, loc, gp, "load id value" );
            if ( g_traceCode )
            {
                Code::emitComment( "<- Id" );
            }
            break;

        case OpK:
            if ( g_traceCode )
            {
                Code::emitComment( "-> Op" );
            }
            p1 = tree->m_child[0];
            p2 = tree->m_child[1];
            /* gen code for ac = left arg */
            cGen( p1 );
            /* gen code to push left operand */
            Code::emitRM( "ST", ac, s_tmpOffset--, mp, "op: push left" );
            /* gen code for ac = right operand */
            cGen( p2 );
            /* now load left operand */
            Code::emitRM( "LD", ac1, ++s_tmpOffset, mp, "op: load left" );
            switch ( tree->m_attr.u_op )
            {
                case PLUS:
                    Code::emitRO( "ADD", ac, ac1, ac, "op +" );
                    break;

                case MINUS:
                    Code::emitRO( "SUB", ac, ac1, ac, "op -" );
                    break;

                case TIMES:
                    Code::emitRO( "MUL", ac, ac1, ac, "op *" );
                    break;

                case OVER:
                    Code::emitRO( "DIV", ac, ac1, ac, "op /" );
                    break;

                case LT:
                    Code::emitRO( "SUB", ac, ac1, ac, "op <" );
                    Code::emitRM( "JLT", ac, 2, pc, "br if true" );
                    Code::emitRM( "LDC", ac, 0, ac, "false case" );
                    Code::emitRM( "LDA", pc, 1, pc, "unconditional jmp" );
                    Code::emitRM( "LDC", ac, 1, ac, "true case" );
                    break;

                case EQ:
                    Code::emitRO( "SUB", ac, ac1, ac, "op ==" );
                    Code::emitRM( "JEQ", ac, 2, pc, "br if true" );
                    Code::emitRM( "LDC", ac, 0, ac, "false case" );
                    Code::emitRM( "LDA", pc, 1, pc, "unconditional jmp" );
                    Code::emitRM( "LDC", ac, 1, ac, "true case" );
                    break;

                default:
                    Code::emitComment( "BUG: Unknown operator" );
                    break;
            }
            if ( g_traceCode )
            {
                Code::emitComment( "<- Op" );
            }
            break;

        default:
            break;
    }
}

/* Procedure cGen recursively generates code by
 * tree traversal
 */
void
CodeGenerator::cGen( TreeNode *tree )
{
    if ( tree != NULL )
    {
        switch ( tree->m_nodekind )
        {
            case StmtK:
                genStmt( tree );
                break;

            case ExpK:
                genExp( tree );
                break;

            default:
                break;
        }
        cGen( tree->m_sibling );
    }
}

/* Procedure codeGen generates code to a code
 * file by traversal of the syntax tree. The
 * second parameter (codefile) is the file name
 * of the code file, and is used to print the
 * file name as a comment in the code file
 */
void
CodeGenerator::codeGen( TreeNode *syntaxTree, char *codefile )
{
    char *s = new char[strlen( codefile ) + 7];

    strcpy( s, "File: " );
    strcat( s, codefile );
    Code::emitComment( "LJ Compilation to LM code" );
    Code::emitComment( s );
    /* generate standard prelude */
    Code::emitComment( "Standard prelude:" );
    Code::emitRM( "LD", mp, 0, ac, "load maxaddress from location 0" );
    Code::emitRM( "ST", ac, 0, ac, "clear location 0" );
    Code::emitComment( "End of standard prelude." );
    /* generate code for LJ program */
    cGen( syntaxTree );
    /* finish */
    Code::emitComment( "End of execution." );
    Code::emitRO( "HALT", 0, 0, 0, "" );
}
