#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include <fstream>

#include "token.h"

class Generator
{
public:
    // Initializes the code generator with an output file path
    Generator(const char* path);

    // Cleanup
    ~Generator();

    // Writes a given sequence to the output
    void emit(const char* sequence);    

    // Writes a given sequence without adding any space
    void emitTight(const char* sequence);

    // Writes a given token to the output
    void emitToken(Token token);

    // Writes a line ending character
    void emitLineEnd();

    // Concludes the main function in the generated c code
    void emitProgramEnd();

    // Special case emit for writing a variable print string
    void emitIdentifierPrint(std::string identifier);
private:
    // The file object for writing
    std::ofstream m_file;

    // Tracks if we are at the start of a line. This is used to prevent a space
    // from being added to the start of each line. While this isn't strictly 
    // necessary, it does result in a slightly nicer/cleaner output
    bool m_startOfLine = true;

    // Writes the initial output. That is to say, this writes the necessary 
    // C includes and the start of the main method, which is where the
    // entirety of our generated code goes
    void emitProgramStart();

    // emits a keyword to the output
    void emitKeyword(Token token);

    // emits an operator to the output
    void emitOperator(Token token);
};

#endif
