/**************************************************************************************
 *                                   LJ compiler                                      *
 *                                                                                    *
 * (c) copyright 2011, lmh                                                            *
 * All rights reserved                                                                *
 *                                                                                    *
 * File   : lexer.h                                                                   *
 * Details: The lexer interface for the LJ compiler                                   *
 *                                                                                    *
 * Name   : lmh                                                                       *
 * Date   : 2011.4.21                                                                 *
 * Version: 0.0                                                                       *
 **************************************************************************************/

#ifndef LEXER_H
#define LEXER_H

/* MAXTOKENLEN is the maximum size of a token */
#define MAXTOKENLEN 40

/* BUFLEN = length of the input buffer for source code line */
#define BUFLEN 256

/* g_tokenString array stores the lexeme of each token */
extern char g_tokenString[MAXTOKENLEN + 1];

class Lexer
{
public:
     Lexer( void );
    ~Lexer( void );

    static TokenType getToken( void );

private:
    static int       getNextChar( void );
    static void      ungetNextChar( void );
    static TokenType reservedLookup( char *s );

    static char s_lineBuf[BUFLEN];    // hold the current line
    static int  s_linepos;            // current position in LineBuf
    static int  s_bufsize;            // current size of buffer string
    static bool s_EOF_Flag;
};

#endif // LEXER_H
