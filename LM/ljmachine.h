/**************************************************************************************
 *                                   LJ compiler                                      *
 *                                                                                    *
 * (c) copyright 2011, lmh                                                            *
 * All rights reserved                                                                *
 *                                                                                    *
 * File   : ljmachine.h                                                               *
 * Details: The LM ("LJ Machine") computer interface for the LJ compiler              *
 *                                                                                    *
 * Name   : lmh                                                                       *
 * Date   : 2011.5.27                                                                 *
 * Version: 0.0                                                                       *
 **************************************************************************************/

#ifndef LJMACHINE_H
#define LJMACHINE_H

#include <cstdio>

extern FILE *g_code;

#define IADDR_SIZE 1024    /* increase for large programs */
#define DADDR_SIZE 1024    /* increase for large programs */
#define NO_REGS       8
#define PC_REG        7
#define LINESIZE    121
#define WORDSIZE     20

typedef enum
{
    opclRR,    // reg operands r, s, t
    opclRM,    // reg r, mem d+s
    opclRA     // reg r, int d+s

} OPCLASS;

typedef enum
{
    /* RR instructions */
    opHALT,    // RR    halt,operands must be zero
    opIN,      // RR    read into reg(r); s and t are ignored
    opOUT,     // RR    write from reg(r), s and t are ignored
    opADD,     // RR    reg( r ) = reg( s ) + reg( t )
    opSUB,     // RR    reg( r ) = reg( s ) - reg( t )
    opMUL,     // RR    reg( r ) = reg( s ) * reg( t )
    opDIV,     // RR    reg( r ) = reg( s ) / reg( t )
    opRRLim,   // limit of RR opcodes

    /* RM instructions */
    opLD,      // RM    reg( r ) = mem( d + reg( s ) )
    opST,      // RM    mem( d + reg( s ) ) = reg( r )
    opRMLim,   // Limit of RM opcodes

    /* RA instructions */
    opLDA,     // RA reg( r ) = d + reg( s )
    opLDC,     // RA reg( r ) = d; reg( s ) is ignored
    opJLT,     // RA if reg( r ) < 0 then reg( 7 ) = d + reg( s )
    opJLE,     // RA if reg( r ) <= 0 then reg( 7 ) = d + reg( s )
    opJGT,     // RA if reg( r ) > 0 then reg( 7 ) = d + reg( s )
    opJGE,     // RA if reg( r ) >= 0 then reg( 7 ) = d + reg( s )
    opJEQ,     // RA if reg( r ) == 0 then reg( 7 ) = d + reg( s )
    opJNE,     // RA if reg( r ) != 0 then reg( 7 ) = d + reg( s )
    opRALim,   // Limit of RA opcodes

} OPCODE;

typedef enum
{
    srOKAY,
    srHALT,
    srIMEM_ERR,
    srDMEM_ERR,
    srZERODIVIDE

} STEPRESULT;

typedef struct
{
    int m_iop;
    int m_iarg1;
    int m_iarg2;
    int m_iarg3;

} INSTRUCTION;

class LJMachine
{
public:
     LJMachine( void );
    ~LJMachine( void );

    static bool readInstructions( void );
    static bool doCommand( void );

private:
    static int        opClass( int );
    static void       writeInstruction( int );
    static void       getCh( void );
    static bool       nonBlank( void );
    static bool       getNum( void );
    static bool       getWord( void );
    static bool       skipCh( char );
    static bool       atEOL( void );
    static bool       error( const char *, int , int );
    static STEPRESULT stepLM( void );

    static int         s_iloc;
    static int         s_dloc;
    static bool        s_traceflag;
    static bool        s_icountflag;
    static INSTRUCTION s_iMem[IADDR_SIZE];
    static int         s_dMem[DADDR_SIZE];
    static int         s_reg[NO_REGS];
    static char        s_inLine[LINESIZE];
    static int         s_lineLen;
    static int         s_inCol;
    static int         s_num;
    static char        s_word[WORDSIZE];
    static char        s_ch;
};

#endif // LJMACHINE_H
