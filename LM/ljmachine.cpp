/**************************************************************************************
 *                                   LJ compiler                                      *
 *                                                                                    *
 * (c) copyright 2011, lmh                                                            *
 * All rights reserved                                                                *
 *                                                                                    *
 * File   : ljmachine.cpp                                                             *
 * Details: The LM ("LJ Machine") computer inmplementaion for the LJ compiler         *
 *                                                                                    *
 * Name   : lmh                                                                       *
 * Date   : 2011.5.27                                                                 *
 * Version: 0.0                                                                       *
 **************************************************************************************/

#include <cstdlib>
#include <cstring>
#include <cctype>
#include "ljmachine.h"

extern FILE *g_code;

/* static values */
static const char *s_opCodeTab[] =
{
    "HALT", "IN", "OUT", "ADD", "SUB", "MUL", "DIV", "????",
    "LD", "ST", "????",    // RR opcodes
    "LDA", "LDC", "JLT", "JLE", "JGT", "JGE", "JEQ", "JNE", "????"    // RA opcodes
};

static const char *s_stepResultTab[] =
{
    "OK", "Halted", "Instruction Memory Fault",
    "Data Memory Fault", "Division by 0"
};

LJMachine::LJMachine( void )
{
}

LJMachine::~LJMachine( void )
{
}

/* LJMachine members */
int         LJMachine::s_iloc       = 0;
int         LJMachine::s_dloc       = 0;
bool        LJMachine::s_traceflag  = false;
bool        LJMachine::s_icountflag = false;
INSTRUCTION LJMachine::s_iMem[IADDR_SIZE];
int         LJMachine::s_dMem[DADDR_SIZE];
int         LJMachine::s_reg[NO_REGS];
char        LJMachine::s_inLine[LINESIZE];
int         LJMachine::s_lineLen;
int         LJMachine::s_inCol;
int         LJMachine::s_num;
char        LJMachine::s_word[WORDSIZE];
char        LJMachine::s_ch;

int
LJMachine::opClass( int c )
{
    if ( c <= opRRLim )
    {
       return opclRR;
    }
    else if ( c <= opRMLim )
    {
        return opclRM;
    }
    else
    {
        return opclRA;
    }
}

void
LJMachine::writeInstruction( int loc )
{
    printf( "%5d: ", loc );
    if ( (loc >= 0) && (loc < IADDR_SIZE) )
    {
        printf( "%6s%3d", s_opCodeTab[s_iMem[loc].m_iop], s_iMem[loc].m_iarg1 );
        switch ( opClass( s_iMem[loc].m_iop ) )
        {
            case opclRR:
                printf( "%1d,%1d", s_iMem[loc].m_iarg2, s_iMem[loc].m_iarg3 );
                break;

            case opclRM:
            case opclRA:
                printf( "%3d(%1d)", s_iMem[loc].m_iarg2, s_iMem[loc].m_iarg3 );
                break;
        }
        printf( "\n" );
    }
}

void
LJMachine::getCh( void )
{
    if ( ++s_inCol < s_lineLen )
    {
        s_ch = s_inLine[s_inCol];
    }
    else
    {
        s_ch = ' ';
    }
}

bool
LJMachine::nonBlank( void )
{
    while ( (s_inCol < s_lineLen) && (s_inLine[s_inCol] == ' ') )
    {
        s_inCol++;
    }
    if ( s_inCol < s_lineLen )
    {
        s_ch = s_inLine[s_inCol];
        return true;
    }
    else
    {
        s_ch = ' ';
        return false;
    }
}

bool
LJMachine::getNum( void )
{
    int  sign;
    int  term;
    bool temp = false;

    s_num = 0;
    do
    {
        sign = 1;
        while ( nonBlank() && ((s_ch == '+') || (s_ch == '-')) )
        {
            temp = false;
            if ( s_ch == '-' )
            {
                sign = -sign;
            }
            getCh();
        }
        term = 0;
        nonBlank();
        while ( isdigit( s_ch ) )
        {
            temp = true;
            term = term * 10 + (s_ch - '0');
            getCh();
        }
        s_num = s_num + (term * sign);

    } while ( nonBlank() && ((s_ch == '+') || (s_ch == '-')) );

    return temp;
}

bool
LJMachine::getWord( void )
{
    int temp   = false;
    int length = 0;

    if ( nonBlank() )
    {
        while ( isalnum( s_ch ) )
        {
            if ( length < WORDSIZE - 1 )
            {
                s_word[length++] = s_ch;
            }
            getCh();
        }
        s_word[length] = '\0';
        temp = (length != 0);
    }

    return temp;
}

bool
LJMachine::skipCh( char c )
{
    int temp = false;

    if ( nonBlank() && (s_ch == c) )
    {
        getCh();
        temp = true;
    }

    return temp;
}

bool
LJMachine::atEOL( void )
{
    return (!nonBlank());
}

bool
LJMachine::error( const char *msg, int lineNo, int instNo )
{
    printf( "Line %d", lineNo );
    if ( instNo >= 0 )
    {
        printf( " (Instruction %d)", instNo );
    }
    printf( "    %s\n", msg );

    return false;
}

bool
LJMachine::readInstructions( void )
{
    int op;
    int arg1, arg2, arg3;
    int loc, regNo, lineNo;

    for ( regNo = 0; regNo < NO_REGS; regNo++ )
    {
        s_reg[regNo] = 0;
    }
    s_dMem[0] = DADDR_SIZE - 1;
    for ( loc = 1; loc < DADDR_SIZE; loc++ )
    {
        s_dMem[loc] = 0;
    }
    for ( loc = 0; loc < IADDR_SIZE; loc++ )
    {
        s_iMem[loc].m_iop   = opHALT;
        s_iMem[loc].m_iarg1 = 0;
        s_iMem[loc].m_iarg2 = 0;
        s_iMem[loc].m_iarg3 = 0;
    }
    lineNo = 0;
    while ( !feof( g_code ) )
    {
        fgets( s_inLine, LINESIZE - 2, g_code );
        s_inCol = 0;
        lineNo++;
        s_lineLen = strlen( s_inLine ) - 1;
        if ( s_inLine[s_lineLen] == '\n' )
        {
            s_inLine[s_lineLen] = '\0';
        }
        else
        {
            s_inLine[++s_lineLen] = '\0';
        }
        if ( (nonBlank()) && (s_inLine[s_inCol] != '*') )
        {
            if ( !getNum() )
            {
                return error( "Bad location", lineNo, -1 );
            }
            loc = s_num;
            if ( loc > IADDR_SIZE )
            {
                return error( "Location too large", lineNo, loc );
            }
            if ( !skipCh( ':' ) )
            {
                return error( "Missing colon", lineNo, loc );
            }
            if ( !getWord() )
            {
                return error( "Missing opcode", lineNo, loc );
            }
            op = opHALT;
            while ( (op < opRALim) && (strncmp( s_opCodeTab[op], s_word, 4 ) != 0) )
            {
                op++;
            }
            if ( strncmp( s_opCodeTab[op], s_word, 4 ) != 0 )
            {
                return error( "Illegal opcode", lineNo, loc );
            }
            switch ( opClass( op ) )
            {
                case opclRR:
                    if ( (!getNum()) || (s_num < 0) || (s_num >= NO_REGS) )
                    {
                        return error( "Bad first register", lineNo, loc );
                    }
                    arg1 = s_num;
                    if ( !skipCh( ',' ) )
                    {
                        return error( "Missing comma", lineNo, loc );
                    }
                    if ( (!getNum()) || (s_num < 0) || (s_num >= NO_REGS) )
                    {
                        return error( "Bad second register", lineNo, loc );
                    }
                    arg2 = s_num;
                    if ( !skipCh( ',' ) )
                    {
                        return error( "Missing comma", lineNo, loc );
                    }
                    if ( (!getNum()) || (s_num < 0) || (s_num >= NO_REGS) )
                    {
                        return error( "Bad third register", lineNo, loc );
                    }
                    arg3 = s_num;
                    break;

                case opclRM:
                case opclRA:
                    if ( (!getNum()) || (s_num < 0) || (s_num >= NO_REGS) )
                    {
                        return error( "Bad first register", lineNo, loc );
                    }
                    arg1 = s_num;
                    if ( !skipCh( ',' ) )
                    {
                        return error( "Missing comma", lineNo, loc );
                    }
                    if ( !getNum() )
                    {
                        return error( "Bad displacement", lineNo, loc );
                    }
                    arg2 = s_num;
                    if ( !skipCh( '(' ) && !skipCh( ',' ) )
                    {
                        return error( "Missing Lparen", lineNo, loc );
                    }
                    if ( (!getNum()) || (s_num < 0) || (s_num >= NO_REGS) )
                    {
                        return error( "Bad second register", lineNo, loc );
                    }
                    arg3 = s_num;
                    break;
            }
            s_iMem[loc].m_iop   = op;
            s_iMem[loc].m_iarg1 = arg1;
            s_iMem[loc].m_iarg2 = arg2;
            s_iMem[loc].m_iarg3 = arg3;
        }
    }

    return true;
}

STEPRESULT
LJMachine::stepLM( void )
{
    INSTRUCTION currentinstruction;
    int         pc;
    int         r, s, t, m;
    bool        ok;

    pc = s_reg[PC_REG];
    if ( (pc < 0) || (pc > IADDR_SIZE) )
    {
        return srIMEM_ERR;
    }
    s_reg[PC_REG] = pc + 1;
    currentinstruction = s_iMem[pc];
    switch ( opClass( currentinstruction.m_iop ) )
    {
        case opclRR:
            r = currentinstruction.m_iarg1;
            s = currentinstruction.m_iarg2;
            t = currentinstruction.m_iarg3;
            break;

        case opclRM:
            r = currentinstruction.m_iarg1;
            s = currentinstruction.m_iarg3;
            m = currentinstruction.m_iarg2 + s_reg[s];
            if ( (m < 0) || (m > DADDR_SIZE) )
            {
                return srDMEM_ERR;
            }
            break;

        case opclRA:
            r = currentinstruction.m_iarg1;
            s = currentinstruction.m_iarg3;
            m = currentinstruction.m_iarg2 + s_reg[s];
            break;
    }
    switch ( currentinstruction.m_iop )
    {
        case opHALT:
            printf( "HALT: %1d,%1d,%1d\n", r, s, t );
            return srHALT;

        case opIN:
            do
            {
                printf( "Enter value for IN instruction: " );
                fflush( stdin );
                fflush( stdout );
                fgets( s_inLine, LINESIZE - 1, stdin );
                s_lineLen = strlen( s_inLine );
                s_inCol = 0;
                ok = getNum();
                if ( !ok )
                {
                    printf( "Illegal value\n" );
                }
                else
                {
                    s_reg[r] = s_num;
                }

            } while ( !ok );
            break;

        case opOUT:
            printf( "OUT instruction prints: %d\n", s_reg[r] );
            break;

        case opADD:
            s_reg[r] = s_reg[s] + s_reg[t];
            break;

        case opSUB:
            s_reg[r] = s_reg[s] - s_reg[t];
            break;

        case opMUL:
            s_reg[r] = s_reg[s] * s_reg[t];
            break;

        case opDIV:
            if ( s_reg[t] != 0 )
            {
                s_reg[r] = s_reg[s] / s_reg[t];
            }
            else
            {
                return srZERODIVIDE;
            }
            break;

        case opLD:
            s_reg[r] = s_dMem[m];
            break;

        case opST:
            s_dMem[m] = s_reg[r];
            break;

        case opLDA:
            s_reg[r] = m;
            break;

        case opLDC:
            s_reg[r] = currentinstruction.m_iarg2;
            break;

        case opJLT:
            if ( s_reg[r] < 0 )
            {
                s_reg[PC_REG] = m;
            }
            break;

        case opJLE:
            if ( s_reg[r] <= 0 )
            {
                s_reg[PC_REG] = m;
            }
            break;

        case opJGT:
            if ( s_reg[r] > 0 )
            {
                s_reg[PC_REG] = m;
            }
            break;

        case opJGE:
            if ( s_reg[r] >= 0 )
            {
                s_reg[PC_REG] = m;
            }
            break;

        case opJEQ:
            if ( s_reg[r] == 0 )
            {
                s_reg[PC_REG] = m;
            }
            break;

        case opJNE:
            if ( s_reg[r] != 0 )
            {
                s_reg[PC_REG] = m;
            }
            break;
    }

    return srOKAY;
}

bool
LJMachine::doCommand( void )
{
    char cmd;
    int  stepcnt = 0;
    int  printcnt;
    int  stepResult;
    int  regNo, loc;

    do
    {
        printf( "Enter command: " );
        fflush( stdin );
        fflush( stdout );
        fgets( s_inLine, LINESIZE, stdin );
        s_lineLen = strlen( s_inLine );
        s_inCol = 0;
    } while ( !getWord() );
    cmd = s_word[0];
    switch ( cmd )
    {
        case 't':
            s_traceflag = !s_traceflag;
            printf( "Tracing now " );
            if ( s_traceflag )
            {
                printf( "on.\n" );
            }
            else
            {
                printf( "off.\n" );
            }
            break;

        case 'h':
            printf( "Command are:\n" );
            printf( "    s(temp <n>      " \
                "Execute n (default 1) LM instructions\n" );
            printf( "    g(o             "\
                "Execute LM instructions until HALT\n" );
            printf( "    r(egs           "\
                "Print the contents of the registers\n" );
            printf( "    i(Mem <b <n>>   "\
                "Print n s_iMem locations starting at b\n" );
            printf( "    d(Mem <b <n>>   "\
                "Print n s_dMem locations starting at b\n" );
            printf( "    t(race          "\
                "Toggle instruction trace\n" );
            printf( "    p(rint          "\
                "Toggle print of total instructions executed"\
                " ('go' only)\n" );
            printf( "    c(lear          "\
                "Reset simulator for new execution of program\n" );
            printf( "    h(elp           "\
                "Cause this list of commands to be printed\n" );
            printf( "    q(uit           "\
                "Terminate the simulation\n" );
            break;

        case 'p':
            s_icountflag = !s_icountflag;
            printf( "Printing instrucion count now " );
            if ( s_icountflag )
            {
                printf( "on.\n" );
            }
            else
            {
                printf( "off.\n" );
            }
            break;

        case 's':
            if ( atEOL() )
            {
                stepcnt = 1;
            }
            else if ( getNum() )
            {
                stepcnt = abs( s_num );
            }
            else
            {
                printf( "Step count?\n" );
            }
            break;

        case 'g':
            stepcnt = 1;
            break;

        case 'r':
            for ( int i = 0; i < NO_REGS; i++ )
            {
                printf( "%1d: %4d    ", i, s_reg[i] );
                if ( (i % 4) == 3 )
                {
                    printf( "\n" );
                }
            }
            break;

        case 'i':
            printcnt = 1;
            if ( getNum() )
            {
                s_iloc = s_num;
                if ( getNum() )
                {
                    printcnt = s_num;
                }
            }
            if ( !atEOL() )
            {
                printf( "Instruction locations?\n" );
            }
            else
            {
                while ( (s_iloc >= 0) && (s_iloc < IADDR_SIZE) && (printcnt > 0) )
                {
                    writeInstruction( s_iloc );
                    s_iloc++;
                    printcnt--;
                }
            }
            break;

        case 'd':
            printcnt = 1;
            if ( getNum() )
            {
                s_dloc = s_num;
                if ( getNum() )
                {
                    printcnt = s_num;
                }
            }
            if ( !atEOL() )
            {
                printf( "Data locations?\n" );
            }
            else
            {
                while ( (s_dloc >= 0) && (s_dloc < DADDR_SIZE) && (printcnt > 0) )
                {
                    printf( "%5d: %5d\n", s_dloc, s_dMem[s_dloc] );
                    s_dloc++;
                    printcnt--;
                }
            }
            break;

        case 'c':
            s_iloc  = 0;
            s_dloc  = 0;
            stepcnt = 0;
            for ( regNo = 0; regNo < NO_REGS; regNo++ )
            {
                s_reg[regNo] = 0;
            }
            s_dMem[0] = DADDR_SIZE - 1;
            for ( loc = 1; loc < DADDR_SIZE; loc++ )
            {
                s_dMem[loc] = 0;
            }
            break;

        case 'q':
            return false;

        default:
            printf( "Command %c unknown.\n", cmd );
            break;
    }
    stepResult = srOKAY;
    if ( stepcnt > 0 )
    {
        if ( cmd == 'g' )
        {
            stepcnt = 0;
            while ( stepResult == srOKAY )
            {
                s_iloc = s_reg[PC_REG];
                if ( s_traceflag )
                {
                    writeInstruction( s_iloc );
                }
                stepResult = stepLM();
                stepcnt++;
            }
            if ( s_icountflag )
            {
                printf( "Number of instructions executed = %dn", stepcnt );
            }
        }
        else
        {
            while ( (stepcnt > 0) && (stepResult == srOKAY) )
            {
                s_iloc = s_reg[PC_REG];
                if ( s_traceflag )
                {
                    writeInstruction( s_iloc );
                }
                stepResult = stepLM();
                stepcnt--;
            }
        }
        printf( "%s\n", s_stepResultTab[stepResult] );
    }

    return true;
}
