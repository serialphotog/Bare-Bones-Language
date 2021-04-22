/*
File: lexer.h
Author: Adam Thompson
Course: CSC 407

Definitions for the lexer/tokenizer.
*/


#ifndef __LEXER_H__
#define __LEXER_H__

#include <cstddef>
#include <fstream>
#include <string>
#include <vector>

#include "token.h"
#include "token_type.h"

// This is a macro to help reduce the repetitive task of creating a token
// and printing the debug output (if enabled).
#define TOKEN(lexeme, type) token = Token(lexeme, type); print_lex(token)

/*
The `Lexer` is responsible for reading a text file of program
code and converting it into the appropriate tokens, which are
later used by the parser.
*/
class Lexer
{
public:
    // Creates a new parser instance from an input file
    Lexer(std::ifstream& file);

    // Gets the next character from the buffer
    void nextChar();


    // Gets the next token in the input
    Token getToken();

    // Looks ahead at the next character in the buffer without incrementing our
    // position within the buffer. Returns the character at the next position
    // within the buffer
    char peek();

    // Gets called when an invalid token has been encountered. Displays an error
    // message and aborts the program.
    // std::string msg -> The message to display in the error message.
    void abort(std::string& msg) const;

private:
    // The input buffer
    std::vector<char> m_inputBuffer;

    // The size of the input buffer
    size_t m_bufferLength;

    // The current position in the input buffer
    int m_currentPos;

    // The current character
    char m_currentChar;

    // Skips whitespace characters in the input buffer
    void skipWhitespace();

    // Skips comments in the input buffer
    void skipComments();

    // Checks if a given string is a keyword. If it is, this will return a
    // T_{KEYWORD} token, otherwise it returns a T_IDENTIFIER token
    Token checkIfKeyword(std::string& lexeme);

    // This is a helper that gets the current character and the next character
    // as a string. This also advances our position in the buffer.
    // This is used for building multi-character operators (i.e. ==, >=, etc.)
    // to avoid having to write the same few lines of code each time.
    std::string getTwoCharToken();
};

#endif
