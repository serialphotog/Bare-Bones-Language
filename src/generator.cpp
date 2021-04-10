#include "generator.h"

#include <cstdlib>
#include <iostream>

#include "bb.h"
#include "token_type.h"

Generator::Generator(const char* path)
{
    // Open the output file
    m_file.open(path);
    if (!m_file.is_open())
    {
        // Failed to create the output file
        std::cerr << "Failed to open the output " << path << std::endl;
        std::cerr << "Aborting..." << std::endl;
        exit(-1);
    }   

    // Write the necessary start C code for our generated output
    emitProgramStart();
}

Generator::~Generator()
{
    // Close the output file
    m_file.close();
}

void Generator::emitProgramStart()
{
    // Write the necessary includes
    m_file << "#include <stdio.h>\n";

#ifdef PRETTY_PRINT
    // Enable an additional newline for readability when the pretty print 
    // option is enabled in `bb.h`
    m_file << "\n";
#endif

    // Start the main function
    m_file << "void main() {";

#ifdef PRETTY_PRINT
    // The pretty print option is enabled in `bb.h`. Add a newline for easier
    // to read (and thus debug) output
    m_file << "\n";
    m_startOfLine = true;
#endif
}

void Generator::emitProgramEnd()
{
    m_file << "}";

    // Ensure the output is fully flushed
    m_file.flush();
}

void Generator::emitIdentifierPrint(std::string identifier)
{
    emitTight("\"%d\", ");
    emitTight(identifier.c_str());
}

void Generator::emit(const char* sequence)
{
    // Write a space if this isn't the start of a line
    if (!m_startOfLine)
        m_file << " ";
    else
    {
#ifdef PRETTY_PRINT
    // The pretty print option is enable in `bb.h`. Add a \t for easier to 
    // read output
    m_file << "\t";
#endif
    }

    // Write the output
    m_file << sequence;
    m_startOfLine = false;

#ifdef FORCE_GENERATOR_FLUSH
    // The option to force flushing the output is enabled in `bb.h` 
    m_file.flush();
#endif
}

void Generator::emitTight(const char* sequence)
{
    m_file << sequence;

#ifdef FORCE_GENERATOR_FLUSH
    // The force flush option is enabled in `bb.h`
    m_file.flush();
#endif
}

void Generator::emitLineEnd()
{
    m_file << ";";

#ifdef PRETTY_PRINT
    // The pretty print option is enabled in `bb.h`, add a newline for easier
    // output debugging
    m_file << "\n";
    m_startOfLine = true;
#endif

#ifdef FORCE_GENERATOR_FLUSH
    // The option to force flushing the output is enabled in `bb.h` 
    m_file.flush();
#endif
}

void Generator::emitToken(Token token)
{
    if (Token::isKeyword(token))
    {
        // Emit a keyword to the output
        emitKeyword(token);
    }
    else if (Token::isKind(token, T_IDENT))
    {
        // Emit an identifier to the output
        emit(token.lexeme().c_str());
    } 
    else if (Token::isOperator(token))
    {
        // Emit an operator to the output
        emitOperator(token);
    } 
    else 
    {
        // Just write the token as-is
        emit(token.lexeme().c_str());
    }
}

void Generator::emitKeyword(Token token)
{
    switch (token.type())
    {
        // Most of the keywords can be written as-is as they are the same as
        // they are in our output language (C).
        case T_IF:
        case T_ELSE:
        case T_FOR:
        case T_WHILE:
            emit(token.lexeme().c_str());
            break;
        // A few need special treament, however
        case T_LET:
            // In the case of our simple language, the only data type is `int`,
            // so this is a very easy case to implement
            emit("int");
            break;
        case T_PRINT:
            // Handles the print keyword
            emit("printf");
            break;
    }
}

void Generator::emitOperator(Token token)
{
     switch (token.type())
     {
        // Handle the operators that are identical to thos in C
        default:
           emit(token.lexeme().c_str()); 
     }
}
