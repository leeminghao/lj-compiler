/**************************************************************************************
 *                                   LJ compiler                                      *
 *                                                                                    *
 * (c) copyright 2011, lmh                                                            *
 * All rights reserved                                                                *
 *                                                                                    *
 * File   : symtab.cpp                                                                *
 * Details: Symbol table implementation for the LJ compiler                           *
 *                                                                                    *
 * Name   : lmh                                                                       *
 * Date   : 2011.5.12                                                                 *
 * Version: 0.0                                                                       *
 **************************************************************************************/

#include "globals.h"
#include "symtab.h"

SymTab::SymTab( void )
{
}

SymTab::~SymTab( void )
{
}

BucketList SymTab::s_hashTable[SIZE];    // the hash table

/* the hash function */
int
SymTab::hash( const char *key )
{
    int temp = 0;

    for ( int i = 0; key[i] != '\0'; i++ )
    {
        temp = ((temp << SHIFT) + key[i]) % SIZE;
    }

    return temp;
}

/* Procedure insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void
SymTab::insert( char *name, int lineno, int loc )
{
    int        h = hash( name );
    BucketList l = s_hashTable[h];

    while ( (l != NULL) && (strcmp( name, l->m_name ) != 0 ) )
    {
        l = l->m_next;
    }
    if ( l == NULL )    // variable not yet in table
    {
        l = new (struct BucketListRec);
        if ( l == NULL )
        {
            fprintf( stderr, "Out of memory allocate struct BuckeyListRec" );
            return;
        }
        l->m_name  = name;
        l->m_lines = new (struct LineListRec);
        if ( l->m_lines == NULL )
        {
            fprintf( stderr, "Out of memory allocate struct LineListRec" );
            return;
        }
        l->m_lines->m_lineno = lineno;
        l->m_memloc          = loc;
        l->m_lines->m_next   = NULL;
        l->m_next            = s_hashTable[h];
        s_hashTable[h]       = l;
    }
    else    // found in table, so just add line number
    {
        LineList t = l->m_lines;
        while ( t->m_next != NULL )
        {
            t = t->m_next;
        }
        t->m_next = new (struct LineListRec);
        if ( t->m_next == NULL )
        {
            fprintf( stderr, "Out of memory allocate struct LineListRec" );
            return;
        }
        t->m_next->m_lineno = lineno;
        t->m_next->m_next   = NULL;
    }
}

/* Function lookup returns the memory
 * location of a variable or -1 if not found
 */
int
SymTab::lookup( char *name )
{
    int        h = hash( name );
    BucketList l = s_hashTable[h];

    while ( (l != NULL) && (strcmp( name, l->m_name ) != 0) )
    {
        l = l->m_next;
    }
    if ( l == NULL )
    {
        return -1;
    }
    else
    {
        return l->m_memloc;
    }
}

/* Procedure printSymTab prints a formatted
 * listing of the symbol table contents
 * to the listing file
 */
void
SymTab::printSymTab( FILE *listing )
{
    fprintf( g_listing, "Variable Name    Location    Line Numbers\n" );
    fprintf( g_listing, "-------------    --------    ------------\n" );
    for ( int i = 0; i < SIZE; i++ )
    {
        if ( s_hashTable[i] != NULL )
        {
            BucketList l = s_hashTable[i];

            while ( l != NULL )
            {
                LineList t = l->m_lines;

                fprintf( listing, "%-14s ", l->m_name );
                fprintf( listing, "%-8d  ", l->m_memloc );
                while ( t != NULL )
                {
                    fprintf( g_listing, "%4d ", t->m_lineno );
                    t = t->m_next;
                }
                fprintf( listing, "\n" );
                l = l->m_next;
            }
        }
    }
}
