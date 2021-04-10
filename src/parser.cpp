#include "parser.h"

#include "bb.h"

#include <algorithm>
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
            // Ensure that the variable has been previously declared
            if (variableHasBeenDeclared(m_currentToken.lexeme()))
            {
                // Output the identifier and advance the parser
                m_generator->emitToken(m_currentToken);
                nextToken();
                assignment();
            }
            else 
            {
                // Attempt to assign a value to an undeclared variable
                abort("Attempt to assign a value to an undeclared variable.");
            }
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
        // Check that we aren't trying to redeclare a variable
        if (!variableHasBeenDeclared(m_currentToken.lexeme()))
        {
            // Write the identifier to the output
            m_generator->emitToken(m_currentToken);

            // Add the variable to the variable map
            pushVariable(m_currentToken.lexeme());

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
            // This is an attempt to re-declare a variable, which is not
            // allowed
            abort("Attempt to redclare a variable.");
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

        // Here we can either have a literal value or an arithmetic expression
        if (Token::isKind(m_currentToken, T_NUM))
            numeric_value(); // Attempt to parse as a numeric value

        // Handle arithmetic expressions
        while (Token::isOperator(m_currentToken))
        {
            // Emit the operator and advance the parser
            m_generator->emitToken(m_currentToken);
            nextToken();

            // Handle the arithmetic expression
            arithmetic_expression();
        }

        // Ensure we end with a ';'
        endl();
    }
    else 
    {
        // Invalid assignment, expected a '='
        abort("Expected an '=' for the assignment.");
    }
}

void Parser::arith_helper()
{
    if (Token::isKind(m_currentToken, T_IDENT) || 
        Token::isKind(m_currentToken, T_NUM))
    {
        if (Token::isKind(m_currentToken, T_NUM))
        {
            numeric_value();
        }
        else
        {
            // Ensure that the identifier has been previously declared
            if (variableHasBeenDeclared(m_currentToken.lexeme()))
            {
                // emit the token and advance the parser
                m_generator->emitToken(m_currentToken);
                nextToken();
            }
            else
            {
                // Error, attempt to reference an undeclared variable
                abort("Variable does not exist.");
            }
        }
    }
    else if (Token::isKind(m_currentToken, T_LPAREN))
    {
        // Emit the parenthesis and advance the parser
        m_generator->emitToken(m_currentToken);
        nextToken();

        // Check for an expression
        arithmetic_expression();

        // Check for the closing parenthesis
        if (Token::isKind(m_currentToken, T_RPAREN))
        {
            // Emit the parenthesis and advance the parser
            m_generator->emitToken(m_currentToken);
            nextToken();
        }
        else
        {
            // Error, expected a ')'
            abort("Expected a R_PAREN");
        }
    }
    else
    {
        // Error, malformed expression
        abort("Malformed arithmetic expression.");
    }
}

void Parser::arithmetic_expression()
{
    print_parse("<arithmetic_expression>");
    
    arith_helper();
    
    // recursive arithmetic expression
    while (Token::isOperator(m_currentToken))
    {
        // Emit the operator, advance the parser, and continue parsing the 
        // arithmetic expression
        m_generator->emitToken(m_currentToken);
        nextToken();
        arith_helper();
    }
}

void Parser::numeric_value()
{
    print_parse("<numeric_value>");

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

bool Parser::variableHasBeenDeclared(std::string var) const
{
    // Search the variable map for the given key
    return std::find(m_variableMap.begin(), m_variableMap.end(), 
        var) != m_variableMap.end();
}

void Parser::pushVariable(std::string var)
{
    m_variableMap.push_back(var);
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
