/*
File: parser.cpp
Author: Adam Thompson
Course: CSC 407

Contains the implementation for the `Parser` class. 
*/


#include "parser.h"

#include "bb.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <sstream>
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
    // Emit the generated program to disk
    m_generator->emitProgram(m_variableMap);
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
        case T_WHILE:
            while_loop();
            break;
        case T_DOTIMES:
            dotimes_loop();
            break;
        case T_PRINT:
            output();
            break;
        case T_READ:
            read();
            break;
        case T_IDENT:
            // Ensure that the variable has been previously declared
            if (identifierHasBeenDeclared(m_currentToken.lexeme()))
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

    // The next token should be an identifier
    nextToken();
    if (Token::isKind(m_currentToken, T_IDENT))
    {
        // Check that we aren't trying to redeclare a variable
        if (!identifierHasBeenDeclared(m_currentToken.lexeme()))
        {
            // Add the variable to the variable map
            pushVariable(m_currentToken.lexeme());

            // Store the identifier in case we need to use this as an 
            // assignment 
            Token identifier = m_currentToken;

            // We got an identifier as expected, check if this is an assignment
            // or simply just a declaration
            nextToken();
            if (Token::isKind(m_currentToken, T_EQ))
            {
                // This is an assignment
                m_generator->emitToken(identifier);
                assignment();
            }
            else
            {
                // This is just a declaration, the next token should be a ';'
                endl(false);
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

    // Emit the if
    m_generator->emitToken(m_currentToken);

    // The next token should be a '('
    nextToken();
    if (Token::isKind(m_currentToken, T_LPAREN))
    {
        // Next should be a boolean expression
        m_generator->emitToken(m_currentToken);
        nextToken();
        boolean_expression();

        // Check for the closing ')'
        if (Token::isKind(m_currentToken, T_RPAREN))
        {
            m_generator->emitToken(m_currentToken);

            // Check for the opening of the block
            nextToken();
            if (Token::isKind(m_currentToken, T_LBRACE))
            {
                // Parse the <stmt_list>
                m_generator->emitBlockStart();
                nextToken();
                while (!Token::isKind(m_currentToken, T_RBRACE))
                    statement();
                
                // Check for the closing '}'
                if (Token::isKind(m_currentToken, T_RBRACE))
                {
                    // Get the next token and determine if there is an else clause
                    m_generator->emitBlockEnd();
                    nextToken();
                    if (Token::isKind(m_currentToken, T_ELSE))
                    {
                        // we have an else clause, look for the opening '{'
                        m_generator->emitToken(m_currentToken);
                        nextToken();
                        if (Token::isKind(m_currentToken, T_LBRACE)) 
                        {
                            m_generator->emitBlockStart();
                            nextToken();
                            
                            // Get the next statement and parse the <stmt_list>
                            while (!Token::isKind(m_currentToken, T_RBRACE))
                                statement();
                            
                            // Check for the '}'
                            if (Token::isKind(m_currentToken, T_RBRACE))
                            {
                                // increment the parser
                                m_generator->emitBlockEnd();
                                nextToken();
                            } 
                            else
                            {
                                abort("Expected a '}' token.");
                            }
                        }
                        else
                        {
                            // Error, else clause must be in a block
                            abort("Expected a '{' token.");
                        }
                    }
                }
                else 
                {
                    abort("Expected a '}' token.");
                }
            }
            else 
            {
                // Error, expected a '{'
                abort("Expected a { token.");
            }
        }
        else
        {
            abort("Expected an RPAREN.");
        }
    } 
    else 
    {
        // Error, expected a '(' character
        abort("Expected a LPAREN.");
    }
}

void Parser::while_loop()
{
    print_parse("<while_loop>");

    // The next token should be a '('
    m_generator->emitToken(m_currentToken);
    nextToken();
    if (Token::isKind(m_currentToken, T_LPAREN))
    {
        // Next should be a boolean expression
        m_generator->emitToken(m_currentToken);
        nextToken();
        boolean_expression();

        // Check for the closing ')'
        if (Token::isKind(m_currentToken, T_RPAREN))
        {
            m_generator->emitToken(m_currentToken);
            nextToken();

            // Check for the start of the code block
            if (Token::isKind(m_currentToken, T_LBRACE))
            {
                m_generator->emitBlockStart();
                nextToken();

                // Parse the statement list
                while (!Token::isKind(m_currentToken, T_RBRACE))
                {
                    statement();
                }

                // Check for the closing '}'
                if (Token::isKind(m_currentToken, T_RBRACE))
                {
                    m_generator->emitBlockEnd();
                    nextToken();
                }
                else
                {
                    // Error, expected a '}' token
                    abort("Expected a '}' token.");
                }
            }
            else
            {
                // Error, expected a '{'
                abort("Expected a '{' token.");
            }
        }
        else
        {
            // Error, expected a ')'
            abort("Expected a RPAREN.");
        }
    }
    else
    {
        // Error, expected a '('
        abort("Expected a LPAREN.");
    }
}

void Parser::dotimes_loop()
{
    print_parse("<dotimes_loop>");

    // Next we should have a '('
    nextToken();
    if (Token::isKind(m_currentToken, T_LPAREN))
    {
        // Check for the identifier or numeric value
        nextToken();
        Token nTimes("", T_UNKNOWN);
        if (Token::isKind(m_currentToken, T_IDENT))
        {
            // Check that the identifier has been previously declared
            checkValidIdentifier(m_currentToken);

            // save the identifer for when we generate the loop header in 
            // the output. 
            nTimes = m_currentToken;
        }
        else if (Token::isKind(m_currentToken, T_NUM))
        {
            nTimes = m_currentToken; 
        }
        else
        {
            abort("Expected an identifier or literal value.");
        }

        // Ensure that we have the closing ')' 
        nextToken();
        if (Token::isKind(m_currentToken, T_RPAREN) 
            && !Token::isKind(nTimes, T_UNKNOWN))
        {
            // Emit the loop header
            m_generator->emitDoTimes(nTimes);

            // Next should be the start of a code block
            nextToken();
            if (Token::isKind(m_currentToken, T_LBRACE))
            {
                // Handle the statement list
                m_generator->emitBlockStart();
                nextToken();

                while (!Token::isKind(m_currentToken, T_RBRACE))
                {
                    statement();
                }

                // Check for the '}' token
                if (Token::isKind(m_currentToken, T_RBRACE))
                {
                    m_generator->emitBlockEnd();
                    nextToken();
                }
                else
                {
                    abort("Expected a '}' token.");
                }
            }
            else
            {
                abort("Expected a '{' token.");
            }
        }
        else
        {
            abort("Expected a RPAREN.");
        }
    }
    else
    {
        // Error, expected a '('
        abort("Expected a LPAREN.");
    }
}

void Parser::boolean_expression()
{
    print_parse("<boolean_expression>");

    // A boolean expression can either start with a !, (, identifier, or literal
    // Check for a '!'
    if (Token::isKind(m_currentToken, T_NOT))
    {
        // We can just emit this token and move one
        m_generator->emitToken(m_currentToken);
        nextToken();
    }

    // Check for a '('
    if (Token::isKind(m_currentToken, T_LPAREN))
    {
        // Emit the token, advance the parser, and parse the remainder as a 
        // boolean expression
        m_generator->emitToken(m_currentToken);
        nextToken();
        boolean_expression();
       
        // Check for the closing ')'
        if (Token::isKind(m_currentToken, T_RPAREN))
        {
            // Emit the token, advance the parser
            m_generator->emitToken(m_currentToken);
            nextToken();

            // If we have a comparison operator (<, >, <=, >=, !=, ==)
            // then we need to emit it and parse the remainder as a boolean
            // expression
            if (Token::isComparisonOperator(m_currentToken))
            {
                m_generator->emitToken(m_currentToken);
                nextToken();
                boolean_expression();
            } 
        }
        else
        {
            // Error, unmatched parenthesis
            abort("Expected a RPAREN.");
        }
    }
    else 
    {
        // If we are here then we have either an identifier or a literal
        if (Token::isKind(m_currentToken, T_IDENT))
        {
            // Check that the identifier has been declared
            checkValidIdentifier(m_currentToken);
        }
        else
        {
            if (!Token::isKind(m_currentToken, T_NUM))
            {
                abort("Unexpected token encountered in boolean expression.");
            }
        }

       m_generator->emitToken(m_currentToken);
       nextToken();

       // We should have a comparison operator here
       if (Token::isComparisonOperator(m_currentToken))
       {
           m_generator->emitToken(m_currentToken);
           nextToken();

           // We could optionally have a '!' here
            if (Token::isKind(m_currentToken, T_NOT))
            {
                m_generator->emitToken(m_currentToken);
                nextToken();
            } 

            // We could optionally have a '(' here
            if (Token::isKind(m_currentToken, T_LPAREN))
            {
                // Emit the token, advance the parser, and parse as boolean expression
                m_generator->emitToken(m_currentToken);
                nextToken();
                boolean_expression();

                // Check for the closing ')'
                if (Token::isKind(m_currentToken, T_RPAREN))
                {
                    m_generator->emitToken(m_currentToken);
                    nextToken();
                }
                else 
                {
                    abort("Expected a RPAREN.");
                }
            }
            else 
            {
                // We should have either an identifer or a literal here
                if (Token::isKind(m_currentToken, T_IDENT))
                {
                    // Check that the identifier has been declared
                    checkValidIdentifier(m_currentToken);
                }
                else
                {
                    if (!Token::isKind(m_currentToken, T_NUM))
                    {
                        abort("Unexpected token encountered in boolean expression.");
                    }
                }

                // Emit the token and advance the parser
                m_generator->emitToken(m_currentToken);
                nextToken();
            }
       }
       else 
       {
           abort("Unexpected token encountered in boolean expression. Expected a boolean comparison.");
       }
    }
}

void Parser::isStringOrIdent()
{
    if (!Token::isKind(m_currentToken, T_STRING)
        && !Token::isKind(m_currentToken, T_IDENT))
    {
        abort("Expected a string literal or identifier for print().");
    }        

    if (Token::isKind(m_currentToken, T_IDENT) 
        && !identifierHasBeenDeclared(m_currentToken.lexeme()))
    {
        abort("Attempt to print an undeclared variable.");
    }
}

void Parser::buildPrint(std::stringstream& ss, std::vector<std::string>& idents)
{
    if (Token::isKind(m_currentToken, T_STRING))
    {
        // Simply append the string literal token to the print string
        ss << m_currentToken.lexeme();
    }
    else
    {
        // This is a variable, append the format specifier to the print
        // string and push the identifier to the map vector. The format
        // specifier portion is extremely simple since our language only
        // deals with integers
        ss << "%d";
        idents.push_back(m_currentToken.lexeme());
    }
}

void Parser::output()
{
    print_parse("<output>");

    // emit the print keyword and advance the parser
    m_generator->emitToken(m_currentToken);
    nextToken();

    // Next we should have a L_PAREN
    if (Token::isKind(m_currentToken, T_LPAREN))
    {
        // Emit the parenthesis
        m_generator->emitToken(m_currentToken);
        nextToken();

        // Check that we have either a string or and identifier w/o consuming
        // the token
        isStringOrIdent();

        /* Start building the output string */
        // This vector will store the variables, in order, that need to be printed 
        std::vector<std::string> identifier_map;
        // This will be the builder for the string literal portion of the print
        std::stringstream ss;

        // Build the print string and variable stack
        buildPrint(ss, identifier_map);

        // Consume the token and move on
        nextToken();

        // Handle the case where we print multiple items in one call
        // Example: print("some string", some_var, "another string");
        while (Token::isKind(m_currentToken, T_COMMA))
        {
            // Consume the comma
            nextToken();

            // We should now have a string or identifier
            isStringOrIdent();

            // Expand the output
            buildPrint(ss, identifier_map);

            // Advance the parser
            nextToken();
        }

        // Emit the final print string to the output
        m_generator->emitPrint(ss.str(), identifier_map);

        // Ensure that we have the R_PAREN
        if (Token::isKind(m_currentToken, T_RPAREN))
        {
            // Emit the parenthesis and advance the parser
            m_generator->emitToken(m_currentToken);
            nextToken();
        }
        else
        {
            // Error, expected a R_PAREN
            abort("Expected a R_PAREN for the call to `print`");
        }

        // Ensure that the line ends with a ';'
        endl(true);
    }
    else
    {
        // Error, expected a L_PAREN
        abort("Expected L_PAREN for the call to `print`");
    }
}

void Parser::read()
{
    print_parse("<read>");

    // The next token should be a '('
    nextToken();
    if (Token::isKind(m_currentToken, T_LPAREN))
    {
        // Next should be an identifer
        nextToken();
        if (Token::isKind(m_currentToken, T_IDENT))
        {
            // The identifier needs to have been previously declared
            checkValidIdentifier(m_currentToken);

            // Store the identifier for when we emit the read to the output
            Token identifier = m_currentToken;

            // Ensure that we have the ending ')' and ';'
            nextToken();
            if (Token::isKind(m_currentToken, T_RPAREN))
            {
                nextToken();
                endl(false);
                m_generator->emitRead(identifier);
            }
            else
            {
                // Expected a ')'
                abort("Expected a R_PAREN.");
            }
        }
        else
        {
            // Error, expected an identifier
            abort("Expected an identifier to read into.");
        }
    }
    else
    {
        abort("Expected a L_PAREN.");
    }
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
        else if (Token::isKind(m_currentToken, T_IDENT))
        {
            // Check that the identifier has bee previously declared
            checkValidIdentifier(m_currentToken);
            m_generator->emitToken(m_currentToken);
            nextToken();
        }
        else 
        {
            // Error, expected a literal or identifier
            abort("Expected a literal value or an identifier.");
        }

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
        endl(true);
    }
    else 
    {
        // Invalid assignment, expected a '='
        abort("Expected an '=' for the assignment.");
    }
}

void Parser::factor()
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
            if (identifierHasBeenDeclared(m_currentToken.lexeme()))
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
    
    // There should be some sort of factor here
    factor();
    
    // recursive arithmetic expression
    while (Token::isOperator(m_currentToken))
    {
        // Emit the operator, advance the parser, and continue parsing the 
        // arithmetic expression
        m_generator->emitToken(m_currentToken);
        nextToken();
        factor();
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

void Parser::endl(bool emit)
{
    if (Token::isKind(m_currentToken, T_SEMICOLON))
    {
        // emit a line ending (if needed) and advance the parser
        if (emit)
            m_generator->emitLineEnd();
        nextToken();
    }
    else
    {
        // Error, expected a ';'
        abort("Expected a ';'");
    }
}

void Parser::checkValidIdentifier(Token identifier) const
{
    if (!identifierHasBeenDeclared(identifier.lexeme()))
        abort("Attempt to reference an undeclared identifier.");
}

bool Parser::identifierHasBeenDeclared(std::string var) const
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
    while (Token::isKind(m_currentToken, T_NEWLINE))
        m_currentToken = m_lexer->getToken();
}
