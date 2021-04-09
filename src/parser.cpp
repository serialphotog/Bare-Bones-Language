#include "parser.h"

#include "bb.h"

#include <cstdlib>
#include <iostream>
#include <string>

Parser::Parser(std::shared_ptr<Lexer> lex, std::shared_ptr<Generator> generator) 
    : m_lexer(lex), m_generator(generator) 
{
    nextToken();
}

void Parser::parse()
{
    print_parse("<program>");

    // Run the main parsing loop
    while (!Token::isKind(m_currentToken, T_EOF)) 
    {
        // Ignore newlines
        if (Token::isKind(m_currentToken, T_NEWLINE))
            nextToken();
        else
            statement();
    }

    // If we made it here then we must've successfully parsed the whole program.
    // Write the end of the output
    m_generator->emitProgramEnd();
}

void Parser::statement()
{
    print_parse("<statement>");

    // Check for valid statement keywords
    switch (m_currentToken.type())
    {
        case T_LET:
            declaration();
            break;
        case T_IF:
            if_else();
            break;
        case T_FOR:
            for_loop();
            break;
        case T_WHILE:
            while_loop();
            break;
        case T_PRINT:
            output();
            break;
        case T_IDENT:
            // Output the identifier and advance the parser
            m_generator->emitToken(m_currentToken);
            nextToken();
            assignment();
            break;
        default:
            // Error, invalid statement
            abort("Invalid statement.");
            break;
    }
}

void Parser::declaration()
{
    print_parse("<declaration>");

    // Output the "let"
    m_generator->emitToken(m_currentToken);

    // The next token should be an identifier
    nextToken();
    if (Token::isKind(m_currentToken, T_IDENT))
    {
        // Write the identifier to the output
        m_generator->emitToken(m_currentToken);

        // We got an identifier as expected, check if this is an assignement
        // or simply just a declaration
        nextToken();
        if (Token::isKind(m_currentToken, T_EQ))
        {
            // This is an assignment
            assignment();
        }
        else
        {
            // This is just a declaration, the next token should be a ';'
            endl();
        }
    }
    else 
    {
        // Invalid declaration, no identifier
        abort("Expected an identifier.");
    }
}

void Parser::if_else()
{
    print_parse("<if_else>");
}

void Parser::while_loop()
{
    print_parse("<while_loop>");
}

void Parser::for_loop()
{
    print_parse("<for_loop>");
}

void Parser::output()
{
    print_parse("<output>");
}

void Parser::assignment()
{
    print_parse("<assignment>");

    // The next token should be an '=' and advance the parser
    if (Token::isKind(m_currentToken, T_EQ))
    {   
        // Output the '='
        m_generator->emitToken(m_currentToken);
        nextToken();

        // Check that we have an integer value
        numeric_value();

        // Ensure we end with a ';'
        endl();
    }
    else 
    {
        // Invalid assignment, expected a '='
        abort("Expected an '=' for the assignment.");
    }
}

void Parser::numeric_value()
{
    if (Token::isKind(m_currentToken, T_NUM))
    {
        // Token appears to be a number, validate this is true
        try
        {
            int i = std::stoi(m_currentToken.lexeme());
        }
        catch (std::invalid_argument const &e)
        {
            // Error, this is not a valid integer value
            abort("Invalid numeric value.");
        }
        catch (std::out_of_range const &e)
        {
            // Error, this results in an integer overflow
            abort("Integer overflow resulted.");
        }

        // If we made it this far then we must've had a valid int value
        m_generator->emitToken(m_currentToken);
        nextToken();
    }
    else 
    {
        // Error, expected a numeric value
        abort("Expected a numeric value.");
    }
}

void Parser::endl()
{
    if (Token::isKind(m_currentToken, T_SEMICOLON))
    {
        // emit a line ending and advance the parser
        m_generator->emitLineEnd();
        nextToken();
    }
    else
    {
        // Error, expected a ';'
        abort("Expected a ';'");
    }
}

void Parser::abort(const char* msg) const
{
    std::cerr << "Parsing error on token: " << m_currentToken.lexeme() 
        << std::endl;
    std::cerr << "\tError: " << msg << std::endl;
    std::cerr << "Aborting..." << std::endl;
    exit(-1);
}

void Parser::nextToken() 
{
    m_currentToken = m_lexer->getToken();
}
