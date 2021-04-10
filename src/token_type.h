#ifndef __TOKEN_TYPE_H__
#define __TOKEN_TYPE_H__

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
    T_EQEQ = 205,       // ==
    T_NEQ = 206,        // !=
    T_LT = 207,         // <
    T_GT = 208,         // >
    T_LTEQ = 209,       // <=
    T_GTEQ = 210,       // >=
    T_MOD = 212,        // %

    // Everthing Else
    T_LPAREN = 112,     // (
    T_RPAREN = 113,     // )
    T_LBRACKET = 114,   // [
    T_RBRACKET = 115,   // ]
    T_LBRACE = 116,     // {
    T_RBRACE = 117,     // }
    T_SEMICOLON = 118,  // ;
    T_NOT = 119,        // !

    // Keywords
    T_LET = 300,        // let
    T_IF = 301,         // if
    T_ELSE = 302,
    T_FOR = 303,        // for
    T_WHILE = 304,      // while
    T_PRINT = 305,      // print
};

#endif
