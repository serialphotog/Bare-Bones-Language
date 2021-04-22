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
    static bool isKeyword(Token tok) 
    {
        return tok.type() >= 300;
    }

    // Checks if a token is an operator. This is an easy check since operators
    // are defined to be >= 200 && < 300
    static bool isOperator(Token tok)
    {
        return tok.type() >= 200 && tok.type() < 300;
    }

    // Checks if a token is a comparison operator. This is an easy check since 
    // comparision operators have a value > 120 && < 130
    static bool isComparisonOperator(Token tok) 
    {
        return tok.type() > 120 && tok.type() < 130;
    }

    // Checks if a token is of a given type.
    static bool isKind(Token tok, TokenType type) 
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

