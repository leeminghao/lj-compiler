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
 * Date   : 2011.5.14                                                                 *
 * Version: 0.0                                                                       *
 **************************************************************************************/

#include "globals.h"
#include "symtab.h"
#include "analyze.h"

Analyze::Analyze( void )
{
}

Analyze::~Analyze( void )
{
}

int Analyze::s_location = 0;

/* Procedure traverse is a generic recursive
 * syntax tree traversal routine:
 * it applies prePro in preorder and postProc
 * in postorder to tree pointed to by t
 */
void
Analyze::traverse( TreeNode *t,
              void (* preProc)( TreeNode * ),
              void (* postProc)( TreeNode * ) )
{
    if ( t != NULL )
    {
        preProc( t );
        for ( int i = 0; i < MAXCHILDREN; i++ )
        {
            traverse( t->m_child[i], preProc, postProc );
        }
        postProc( t );
        traverse( t->m_sibling, preProc, postProc );
    }
}

/* nullProc is a do-nothing procedure to
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
void
Analyze::nullProc( TreeNode *t )
{
    if ( t == NULL )
    {
        return;
    }
    else
    {
        return;
    }
}

/* Procedure insertNode inserts
 * identifiers stored in t into
 * the symbol table
 */
void
Analyze::insertNode( TreeNode *t )
{
    switch ( t->m_nodekind )
    {
        case StmtK:
            switch ( t->m_kind.u_stmt )
            {
                case AssignK:
                case ReadK:
                    if ( SymTab::lookup( t->m_attr.u_name ) == -1 )
                    {
                        /* not yet in table, so treat as new definition */
                        SymTab::insert( t->m_attr.u_name, t->m_lineno, s_location++ );
                    }
                    else
                    {
                        /* already in table, so ignore location,
                         * add line number of use only
                         */
                         SymTab::insert( t->m_attr.u_name, t->m_lineno, 0 );
                    }
                    break;

                default:
                    break;
            }
            break;

        case ExpK:
            switch ( t->m_kind.u_exp )
            {
                case IdK:
                    if ( SymTab::lookup( t->m_attr.u_name ) == -1 )
                    {
                        /* not yet in table, so treat as new definition */
                        SymTab::insert( t->m_attr.u_name, t->m_lineno, s_location++ );
                    }
                    else
                    {
                        /* already in table, so ignore location,
                         * add line number of use only
                         */
                         SymTab::insert( t->m_attr.u_name, t->m_lineno, 0 );
                    }
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }
}

/* Function buildSymtab constructs the symbol
 * table by preorder traversal of the syntax tree
 */
void
Analyze::buildSymtab( TreeNode *syntaxTree )
{
    traverse( syntaxTree, insertNode, nullProc );
    if ( g_traceAnalyze )
    {
        fprintf( g_listing, "\nSymbol table:\n\n" );
        SymTab::printSymTab( g_listing );
    }
}

void
Analyze::typeError( TreeNode *t, const char *message )
{
    fprintf( g_listing, "Type error at line %d: %s\n", t->m_lineno, message );
    g_error = true;
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
void
Analyze::checkNode( TreeNode *t )
{
    switch ( t->m_nodekind )
    {
        case ExpK:
            switch ( t->m_kind.u_exp )
            {
                case OpK:
                    if ( (t->m_child[0]->m_type != Integer) ||
                         (t->m_child[1]->m_type != Integer) )
                    {
                        typeError( t, "Op applied to non-integer" );
                    }
                    if ( (t->m_attr.u_op == EQ) || (t->m_attr.u_op == LT) )
                    {
                        t->m_type = Boolean;
                    }
                    else
                    {
                        t->m_type = Integer;
                    }
                    break;

                case ConstK:
                case IdK:
                    t->m_type = Integer;
                    break;

                default:
                    break;
            }
            break;

        case StmtK:
            switch ( t->m_kind.u_stmt )
            {
                case IfK:
                    if ( t->m_child[0]->m_type == Integer )
                    {
                        typeError( t->m_child[0], "if test is not Boolean" );
                    }
                    break;

                case AssignK:
                    if ( t->m_child[0]->m_type != Integer )
                    {
                        typeError( t->m_child[0], "assignment of non-integer value" );
                    }
                    break;

                case WriteK:
                    if ( t->m_child[0]->m_type != Integer )
                    {
                        typeError( t->m_child[0], "write of non-integer value" );
                    }
                    break;

                case RepeatK:
                    if ( t->m_child[1]->m_type == Integer )
                    {
                        typeError( t->m_child[1], "repeat test is not Boolean" );
                    }
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }
}

/* Procedure typeCheck performs type checking
 * by a postorder syntax tree traversal
 */
void
Analyze::typeCheck( TreeNode *syntaxTree )
{
    traverse( syntaxTree, nullProc, checkNode );
}
