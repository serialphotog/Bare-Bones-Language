#include "lexer.h"

#include "bb.h"

#include <cctype>
#include <cstdlib>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>

Lexer::Lexer(std::ifstream& file) 
{
    // Read the input file into a buffer
    //std::ifstream file(path);
    std::ostringstream ss;
    ss << file.rdbuf();
    const std::string& s = ss.str();
    m_inputBuffer = std::vector<char>(s.begin(), s.end());

    // Initialize the lexer state
    m_bufferLength = m_inputBuffer.size();
    m_currentPos = 0;
    m_currentChar = m_inputBuffer[0];
}

void Lexer::nextChar() 
{
    m_currentPos++;
    if (m_currentPos >= m_bufferLength)
        m_currentChar = '\0'; // Marks the end of the input
    else
        m_currentChar = m_inputBuffer[m_currentPos];
}

Token Lexer::getToken()
{
    // Skip comments and whitespace
    skipWhitespace();
    skipComments();
    Token token("", T_UNKNOWN);
    
    // Start by looking at the first character to try and determine what token
    // we are currently looking at. In the case where a character could be the 
    // start of a multi-character token, we look ahead with peek to try and 
    // determine what token this is. This logic is basically just an 
    // implementation of a simple FSM.
    switch (m_currentChar)
    {
        case '+':
           TOKEN(m_currentChar, T_PLUS);
           break; 
        case '-':
            TOKEN(m_currentChar, T_MINUS);
            break;
        case '*':
            TOKEN(m_currentChar, T_MUL);
            break;
        case '/':
            TOKEN(m_currentChar, T_DIV);
            break;
        case '%':
            TOKEN(m_currentChar, T_MOD);
            break;
        case '=':
            // Check if this is part of a '=='
            if (peek() == '=')
            {
                // This is a '==' token
                TOKEN(getTwoCharToken(), T_EQEQ);
            }
            else
            {
                // This is just a '=' token
                TOKEN(m_currentChar, T_EQ);
            }
            break;
        case '>':
            // Check if this is part of a '>='
            if (peek() == '=') 
            {
                // This is '>=' token
                TOKEN(getTwoCharToken(), T_GTEQ);
            }
            else 
            {
                // This is just a '>' token
                TOKEN(m_currentChar, T_GT);
            }
            break;
        case '<':
            // Check if this is part of a '<='
            if (peek() == '=')
            {
                // This is a '<=' token
                TOKEN(getTwoCharToken(), T_LTEQ);
            }
            else 
            {
                // This is just a '<' token
                TOKEN(m_currentChar, T_LT);
            }
            break;
        case '!':
            // Check if this is part of a '!='
            if (peek() == '=')
            {
                // This is a '!=' token 
                TOKEN(getTwoCharToken(), T_NEQ);
            }
            else
            {
                // This is just a '!' token
                TOKEN(m_currentChar, T_NOT);
            }
            break;
        case '(':
            TOKEN(m_currentChar, T_LPAREN);
            break;
        case ')':
            TOKEN(m_currentChar, T_RPAREN);
            break;
        case '[':
            TOKEN(m_currentChar, T_LBRACKET);
            break;
        case ']':
            TOKEN(m_currentChar, T_RBRACKET);
            break;
        case '{':
            TOKEN(m_currentChar, T_LBRACE);
            break;
        case '}':
            TOKEN(m_currentChar, T_RBRACE);
            break;
        case '\n':
            TOKEN(std::string("newline"), T_NEWLINE);
            break;
        case '\0': // EOF marker
            TOKEN(std::string("EOF"), T_EOF);
            break;
        case ';': 
            TOKEN(m_currentChar, T_SEMICOLON);
            break;
        default:
            // Deal with number literals
            if (isdigit(m_currentChar))
            {
                // We are reading the start of a number, since our language is 
                // so primitive that it only recognizes ints, this is pretty 
                // trivial. Simply continue reading until we hit something that 
                // is not a digit.
                int start = m_currentPos; 
                while (isdigit(peek()))
                    nextChar();
                // Build the full number string
                std::stringstream ss;
                for (int i = start; i <= m_currentPos; i++)
                    ss << m_inputBuffer[i]; 
                TOKEN(ss.str(), T_NUM);
                break;
            }
            else if (isalpha(m_currentChar))
            {
                // This token begins with a letter, so it must be an identifier
                // or a keyword. Start by building the full string
                int start = m_currentPos;
                while (isalnum(peek()))
                    nextChar();
                // Build the full string
                std::stringstream ss;
                for (int i = start; i <= m_currentPos; i++)
                    ss << m_inputBuffer[i];
                std::string tokenText = ss.str();

                // Determine if this is an identifier or a keyword
                token = checkIfKeyword(tokenText);
            }
            else 
            {
                // We have no idea what this token is...
                TOKEN("UNKNOWN", T_UNKNOWN);
                // TODO: There should probably be some error reporting here...
                break;
            }
    }

    // Advance the lexer
    nextChar();

    return token;
}

Token Lexer::checkIfKeyword(std::string& lexeme)
{
    // Default to an identifier
    Token token(lexeme, T_IDENT);    

    // Check if this is any of our keywords
    if (lexeme == "let")
    {
        TOKEN(lexeme, T_LET);
    }
    else if (lexeme == "if")
    {
        TOKEN(lexeme, T_IF);
    }
    else if (lexeme == "else")
    {
        TOKEN(lexeme, T_ELSE);
    }
    else if (lexeme == "for")
    {
        TOKEN(lexeme, T_FOR);
    }
    else if (lexeme == "while")
    {
        TOKEN(lexeme, T_WHILE);
    }
    else if (lexeme == "print")
    {
        TOKEN(lexeme, T_PRINT);
    }

    return token;
}

char Lexer::peek()
{
    if (m_currentPos+1 >= m_bufferLength)
        return '\0'; // Marks the end of the input
    return m_inputBuffer[m_currentPos+1];
}

void Lexer::abort(std::string& msg) const
{
    std::cerr << "Lexing error: " << msg << std::endl;
    std::cerr << "Aborting..." << std::endl;
    exit(EXIT_FAILURE);
}

void Lexer::skipWhitespace() 
{
    while (m_currentChar == ' ' || m_currentChar == '\t' 
        || m_currentChar == '\r')
    {
        nextChar();
    }
}

void Lexer::skipComments()
{
    if (m_currentChar == '#')
    {
        while (m_currentChar != '\n')
            nextChar();
    }
}

std::string Lexer::getTwoCharToken()
{
    char prev = m_currentChar;
    nextChar();
    std::stringstream ss;
    ss << prev << m_currentChar;
    return ss.str();
}
