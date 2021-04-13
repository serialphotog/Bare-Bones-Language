#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include <fstream>
#include <sstream>
#include <vector>

#include "token.h"

class Generator
{
public:
    // Initializes the code generator with an output file path
    Generator(const char* path);

    // Cleanup
    ~Generator();

    // Finalizes the program generation and flushes the output ot disk
    // The identifier map is a stack of identifiers that need to be initialized
    // at the start of the program.
    void emitProgram(const std::vector<std::string>& identifierMap);

    // Writes a given token to the output
    void emitToken(Token token);

    // Emits the start of a code block
    void emitBlockStart();

    // Emits the end of a code block
    void emitBlockEnd();

    // Writes a line ending character
    void emitLineEnd();

    // Emits the internal portion of a printf for the implementation of our
    // print() call
    void emitPrint(const std::string& str, const std::vector<std::string>& idents);
private:
    // The file object for writing
    std::ofstream m_file;

    // Stores the lines of the output during generation
    std::vector<std::string> m_lines;

    // Used for building individual lines
    std::stringstream m_line;

    // Tracks if we are at the start of a line. This is used to prevent a space
    // from being added to the start of each line. While this isn't strictly 
    // necessary, it does result in a slightly nicer/cleaner output
    bool m_startOfLine = true;

    // Emits a given sequence to the output
    void emit(const char* sequence);    

    // Writes a given sequence without adding any space
    void emitTight(const char* sequence);

    // Emits the identifier initializations
    void emitInitializations(const std::vector<std::string> identifierMap);

    // Emits the generated program output
    void emitOutput();

    // emits a keyword to the output
    void emitKeyword(Token token);

    // emits an operator to the output
    void emitOperator(Token token);
};

#endif
