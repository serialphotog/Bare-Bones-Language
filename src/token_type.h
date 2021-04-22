/*
File: token_type.h
Author: Adam Thompson
Course: CSC 407

This file contains definitions for the various kinds of tokens found in
the Bare Bones Language. 
*/

#ifndef __TOKEN_TYPE_H__
#define __TOKEN_TYPE_H__

// Definitions for the token types in the langauge.
enum TokenType
{
    T_UNKNOWN = -2, // Unknown token
    T_EOF = -1,     // end of file 
    T_NEWLINE = 0,  // Newline
    T_NUM = 1,      // Number
    T_STRING = 2,   // String
    T_IDENT = 3,    // Identifier

    // Operators
    T_EQ = 200,         // =
    T_PLUS = 201,       // +
    T_MINUS = 202,      // -
    T_DIV = 203,        // /
    T_MUL = 204,        // *
    T_MOD = 212,        // %

    // Everthing Else
    T_LPAREN = 112,     // (
    T_RPAREN = 113,     // )
    T_LBRACKET = 114,   // [
    T_RBRACKET = 115,   // ]
    T_LBRACE = 116,     // {
    T_RBRACE = 117,     // }
    T_SEMICOLON = 118,  // ;
    T_COMMA = 119,      // ,

    // Boolean operations
    T_NOT = 120,        // !
    T_EQEQ = 121,       // ==
    T_LT = 122,         // <
    T_GT = 123,         // >
    T_LTEQ = 124,       // <=
    T_GTEQ = 125,       // >=
    T_AND = 126,        // and
    T_OR = 127,         // or 
    T_NEQ = 128,        // !=

    // Keywords
    T_LET = 300,        // let
    T_IF = 301,         // if
    T_ELSE = 302,
    T_WHILE = 303,      // while
    T_PRINT = 304,      // print
    T_DOTIMES = 305,    // dotimes
    T_READ = 306,       // read
};

#endif
