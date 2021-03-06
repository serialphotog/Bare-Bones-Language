/*
File: parser.h
Author: Adam Thompson
Course: CSC 407

Contains the definitions for the langauge parser.

The grammar for the bare bones language (bb) is as follows:

// Program structure
<program> --> <statement_list>
<statement_list> --> <statement>; | <statement>; <statement_list>
<statement> --> <declaration> | <assignment> | <if_else> | <loop> |
    <input> | <output>

// Variable declaration and assignment
<declaration> --> let <identifier>; | let <assignment>
<assignment> --> <identifier> = <numeric_value> |
    <arithmetic_expression>;

// Expressions
<arithmetic_expression> --> <numeric_value> <math_op> <factor>; |
    <numeric_value> <math_op> ( <arithmetic_expression> ); |
    <numeric_value> <math_op> <arithmetic_expression>
<factor> --> <identifier> | <numeric_value> | ( <arithmetic_expression> )
<boolean_expression> --> [!] (<identifier> | <literal>) <comparison_operator> (<identifier> | <literal>)
    | [!] <boolean_expression> ) | [!] <boolean_expression> <comparison_operator> <boolean_expression>
    | [!] (<identifier> | <literal>) <comparison_operator> <boolean_expression>

// Control Structures
<if_else> --> if (<boolean_expression>) { <statement_list> } else { <statement_list> } 
    | if (<boolean_expression>) { <statement_list> }
<loop> --> <while_loop> | <dotimes_loop>
<while_loop> --> while (<boolean_expression>) { <statement_list> }
<dotimes_loop> --> dotimes (<numeric_value>) { <statement_list> } |
    dotimes (<identifier>) { <statement_list> } |

// I/O
<output> --> print(<output_seq>); 
<output_seq> --> <string> | <identifier> | <string, <identifier>
<input> --> read(<identifier>);

// Base constructs
<identifier> --> String of characters 
<numeric_value> --> any numeric value
<math_op> --> + | - | * | / | %
<comparison_operator> --> == | < | > | <= | >= | and | or 
*/

#ifndef __PARSER_H__
#define __PARSER_H__

#include <memory>
#include <vector>

#include "generator.h"
#include "lexer.h"
#include "token.h"
#include "token_type.h"

/*
The `Parser` class implements a recursive decent parser for the Bare
Bones Language. It also triggers the code generator to generate the
compiled code. Currently, the language compiles down to C, however
I have attempted to keep everything abstracted well enough to make
adding other code backends fairly straight-forward. 
*/
class Parser
{
public:
    // Initializes the parser with a Lexer instance
    Parser(std::shared_ptr<Lexer> lex, std::shared_ptr<Generator>);

    // Starts the processing of a program
    // This effectively starts parsing the <program> prodcution of the grammar
    void parse();

private:
    // The lexer instance
    std::shared_ptr<Lexer> m_lexer;

    // The generator instance
    std::shared_ptr<Generator> m_generator;

    // Tracks if a variable with a given name has been declared or not.
    // When we first encounter a variable declaration we place its name in this
    // vector. Since our simple langauge has no concept of variable scope, this
    // simple solution is sufficient for ensuring that variables have been 
    // previously declared.
    std::vector<std::string> m_variableMap;

    // The current token being parsed
    Token m_currentToken;

    // Checks if a variable has been previously declared by searching the 
    // variable map vector
    bool identifierHasBeenDeclared(std::string var) const;

    // Pushes a variable name onto the variable map
    void pushVariable(std::string var);

    // Called when a parsing error occurs
    void abort(const char* msg) const;

    // Gets the next token and updates the look ahead token
    void nextToken();

    /*
     * The following perform parsing on the production rules for the language
     */

    // <statement> --> <declaration> | <assignment> | <if_else> | <loop> |
    //      <input> | <output>
    void statement();

    // <declaration> --> let <identifier>; | let <assignment>
    void declaration();

    // <assignment> --> <identifier> = <numeric_value> |
    //      <arithmetic_expression>;
    void assignment();

    // <if_else> --> if (<boolean_expression>) { <statement_list> } else { <statement_list> } 
    //      if (<boolean_expression>) { <statement_list> }
    void if_else();

    // <while_loop> --> while (<boolean_expression>) { <statement_list> }
    void while_loop();

    // <dotimes_loop> --> dotimes (<numeric_value>) { <statement_list> } |
    //      dotimes(<identifier>) { <statement_list> } |
    void dotimes_loop();

    // <output> --> print(<output_seq>); 
    void output();

    // <input> -- > read(<identifier>);
    void read();

    // <arithmetic_expression> --> <numeric_value> <math_op> <factor>; |
    //      <numeric_value> <math_op> (<arithmetic_expression>); |
    //      <numeric_value> <math_op> <arithmetic_expression>
    void arithmetic_expression();

    // <boolean_expression> --> [!] (<identifier> | <literal>) <comparison_operator> (<identifier> | <literal>)
    //  | [!] <boolean_expression> ) | [!] <boolean_expression> <comparison_operator> <boolean_expression>
    //  | [!](<identifier> | <literal>) < comparison_operator > <boolean_expression>
    void boolean_expression();

    // <factor> --> <identifier> | <numeric_value> | ( <arithmetic_expression> )
    void factor();

    // Checks for any valid numeric (integer) value. 
    void numeric_value(); 

    // Checks for a line ending (semicolon). 
    // The emit flag indicates rather or not we should emit a line ending to the
    // generated output. This is needed because, in some cases, the generator
    // handles the line ending for us.
    void endl(bool emit);

    /*
    * The following are helper functions for parsing the langauge production
    * rules.
    */

    // A helper function to check if a given identifier is valid
    void checkValidIdentifier(Token identifier) const;

    // Helper function for <output> to verify that we have either a string
    // literal or an identifier without consuming the current token.
    void isStringOrIdent();

    // This is a helper to build the output string and identifier stack for 
    // our print(). This information is later used by the generator to output
    // the resulting print call in the output language (C, in our case).
    void buildPrint(std::stringstream& ss, std::vector<std::string>& idents);
};

#endif
