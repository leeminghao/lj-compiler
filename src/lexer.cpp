/**************************************************************************************
 *                                   LJ compiler                                      *
 *                                                                                    *
 * (c) copyright 2011, lmh                                                            *
 * All rights reserved                                                                *
 *                                                                                    *
 * File   : lexer.cpp                                                                 *
 * Details: The lexer implementation for the LJ compiler                              *
 *                                                                                    *
 * Name   : lmh                                                                       *
 * Date   : 2011.4.21                                                                 *
 * Version: 0.0                                                                       *
 **************************************************************************************/

#include "globals.h"
#include "util.h"
#include "lexer.h"

/* state in scanner DFA */
typedef enum
{
    START, INASSIGN, INCOMMENT, INNUM, INID, DONE

} StateType;

/* lexeme of identifier or reserved word */
char g_tokenString[MAXTOKENLEN + 1];

Lexer::Lexer( void )
{
}

Lexer::~Lexer( void )
{
}

char Lexer::s_lineBuf[BUFLEN];
int  Lexer::s_linepos  = 0;
int  Lexer::s_bufsize  = 0;
bool Lexer::s_EOF_Flag = false;

/* getNextChar fetches the next non-blank character
 * from m_lineBuf, reading in a new line if m_lineBuf
 * is exhausted
 */
int
Lexer::getNextChar( void )
{
    if ( s_linepos >= s_bufsize )
    {
        g_lineno++;
        if ( fgets( s_lineBuf, BUFLEN - 1, g_source ) )
        {
            if ( g_echoSource )
            {
                fprintf( g_listing, "%4d: %s", g_lineno, s_lineBuf );
            }
            s_bufsize = strlen( s_lineBuf );
            s_linepos = 0;

            return s_lineBuf[s_linepos++];
        }
        else
        {
            s_EOF_Flag = true;

            return EOF;
        }
    }
    else
    {
        return s_lineBuf[s_linepos++];
    }
}

/* ungetNextChar backtracks one character
 * in m_lineBuf
 */
void
Lexer::ungetNextChar( void )
{
    if ( !s_EOF_Flag )
    {
        s_linepos--;
    }
}

static struct
{
    const char *m_str;
    TokenType   m_tok;

} s_reservedWords[MAXRESERVED] =
{
    { "if"    , IF     }, { "then" , THEN   },
    { "else"  , ELSE   }, { "end"  , END    },
    { "repeat", REPEAT }, { "until", UNTIL  },
    { "read"  , READ   }, { "write", WRITE  }
};

/* lookup an identifier to see if it is a reserved word
 * uses linear search
 */
TokenType
Lexer::reservedLookup( char *s )
{
    for ( int i = 0; i < MAXRESERVED; i++ )
    {
        if ( !strcmp( s, s_reservedWords[i].m_str ) )
        {
            return s_reservedWords[i].m_tok;
        }
    }

    return ID;
}

/* function getToken returns the
 * next token in source file
 */
TokenType
Lexer::getToken( void )
{
    int       tokenStringIndex = 0;     // index for storing into g_tokenString
    TokenType currentToken;             // holds current token to be returned
    StateType state            = START; // current state - always begins at START
    bool      save;                     // flag to indicate save to tokenString

    while ( state != DONE )
    {
        int c = getNextChar();

        save = true;
        switch ( state )
        {
            case START:
                if ( isdigit( c ) )
                {
                    state = INNUM;
                }
                else if ( isalpha( c ) )
                {
                    state = INID;
                }
                else if ( c == ':' )
                {
                    state = INASSIGN;
                }
                else if ( (c == ' ') || (c == '\t') || (c == '\n') )
                {
                    save = false;
                }
                else if ( c == '{' )
                {
                    save  = false;
                    state = INCOMMENT;
                }
                else
                {
                    state = DONE;
                    switch ( c )
                    {
                        case EOF:
                            save         = false;
                            currentToken = ENDFILE;
                            break;

                        case '=':
                            currentToken = EQ;
                            break;

                        case '<':
                            currentToken = LT;
                            break;

                        case '+':
                            currentToken = PLUS;
                            break;

                        case '-':
                            currentToken = MINUS;
                            break;

                        case '*':
                            currentToken = TIMES;
                            break;

                        case '/':
                            currentToken = OVER;
                            break;

                        case '(':
                            currentToken = LPAREN;
                            break;

                        case ')':
                            currentToken = RPAREN;
                            break;

                        case ';':
                            currentToken = SEMI;
                            break;

                        default:
                            currentToken = ERROR;
                            break;
                    }
                }
                break;

            case INCOMMENT:
                save = false;
                if ( c == EOF )
                {
                    state        = DONE;
                    currentToken = ENDFILE;
                }
                else if ( c == '}' )
                {
                    state = START;
                }
                break;

            case INASSIGN:
                state = DONE;
                if ( c == '=' )
                {
                    currentToken = ASSIGN;
                }
                else
                {
                    ungetNextChar();    // backup in the input
                    save         = false;
                    currentToken = ERROR;
                }
                break;

            case INNUM:
                if ( !isdigit( c ) )
                {
                    ungetNextChar();    // backup in the input
                    save         = false;
                    state        = DONE;
                    currentToken = NUM;
                }
                break;

            case INID:
                if ( !isalpha( c ) )
                {
                    ungetNextChar();    // backup in the input
                    save         = false;
                    state        = DONE;
                    currentToken = ID;
                }
                break;

            case DONE:
            default:    // should never happen
                fprintf( g_listing, "Lexer Bug: state = %d\n", state );
                state        = DONE;
                currentToken = ERROR;
                break;
        }

        if ( (save) && (tokenStringIndex <= MAXTOKENLEN) )
        {
            g_tokenString[tokenStringIndex++] = (char)c;
        }
        if ( state == DONE )
        {
            g_tokenString[tokenStringIndex] = '\0';
            if ( currentToken == ID )
            {
                currentToken = reservedLookup( g_tokenString );
            }
        }
    }

    if ( g_traceScan )
    {
        fprintf( g_listing, "\t%d: ", g_lineno );
        Util::printToken( currentToken, g_tokenString );
    }

    return currentToken;
}
