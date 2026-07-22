/*
File: token.h
Author: Adam Thompson
Course: CSC 407
*/


#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <string>

#include "token_type.h"

/*
The `Token` class is a simple wrapper around a single token within the
langauge. 
*/
class Token
{
public:
    // Initializes this token with a lexeme and a type
    Token(std::string lexeme, TokenType type) : m_lexeme(lexeme),
        m_type(type) { }

    // Initialization that takes a single character for the lexme
    Token(char lexeme, TokenType type) :
        Token(std::string(1, lexeme), type) { }

    // Allow creating an empty Token
    Token() { }

    // Checks if a token is a keyword. This is an easy check since keywords are
    // defined to have a value >= 300
    static bool isKeyword(const Token& tok)
    {
        return tok.type() >= 300;
    }

    // Checks if a token is an arithmetic operator.
    static bool isArithmeticOperator(const Token& tok)
    {
        switch (tok.type())
        {
            case T_PLUS:
            case T_MINUS:
            case T_DIV:
            case T_MUL:
            case T_MOD:
                return true;
            default:
                return false;
        }
    }

    // Checks if a token is the assignment operator.
    static bool isAssignmentOperator(const Token& tok)
    {
        return tok.type() == T_EQ;
    }

    // Checks if a token is a binary logical operator.
    static bool isLogicalOperator(const Token& tok)
    {
        return tok.type() == T_AND || tok.type() == T_OR;
    }

    // Checks if a token is a comparison operator.
    static bool isComparisonOperator(const Token& tok)
    {
        switch (tok.type())
        {
            case T_EQEQ:
            case T_LT:
            case T_GT:
            case T_LTEQ:
            case T_GTEQ:
            case T_NEQ:
                return true;
            default:
                return false;
        }
    }

    // Checks if a token is of a given type.
    static bool isKind(const Token& tok, TokenType type)
    {
        return tok.type() == type; 
    }

    // Gets the lexeme for this token
    std::string lexeme() const { return m_lexeme; }

    // Gets the type for this token
    TokenType type() const { return m_type; }
    
private:
    // The lexeme this token represents
    std::string m_lexeme;

    // The type of token this is
    TokenType m_type;
};

#endif
