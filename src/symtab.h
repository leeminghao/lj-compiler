/**************************************************************************************
 *                                   LJ compiler                                      *
 *                                                                                    *
 * (c) copyright 2011, lmh                                                            *
 * All rights reserved                                                                *
 *                                                                                    *
 * File   : symtab.h                                                                  *
 * Details: Symbol table interface for the LJ compiler                                *
 *                                                                                    *
 * Name   : lmh                                                                       *
 * Date   : 2011.5.12                                                                 *
 * Version: 0.0                                                                       *
 **************************************************************************************/

#ifndef SYMTAB_H
#define SYMTAB_H

#include <cstdio>

/* SIZE is the size of the hash table */
#define SIZE 211

/* SHIFT is the power of two used as multiplier */
#define SHIFT 4

/* the list of line numbers of the source
 * code in which a variable is referenced
 */
typedef struct LineListRec
{
    int                 m_lineno;
    struct LineListRec *m_next;

} *LineList;

/* The record in the bucket lists for
 * each variable, including name,
 * assigned memory location, and
 * the list of line numbers in which
 * it appears in the source code
 */
typedef struct BucketListRec
{
    char                 *m_name;
    LineList              m_lines;
    int                   m_memloc;    // memory location for variable
    struct BucketListRec *m_next;

} *BucketList;

class SymTab
{
public:
     SymTab( void );
    ~SymTab( void );

    static void insert( char *, int , int );
    static int  lookup( char * );
    static void printSymTab( FILE * );

private:
    static int hash( const char * );

    static BucketList s_hashTable[SIZE];
};

#endif // SYMTAB_H
