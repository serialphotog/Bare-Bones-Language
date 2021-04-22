/*
File: generator.h
Author: Adam Thompson
Course: CSC 407

This file contains definitions for the code generator.
*/


#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include <fstream>
#include <sstream>
#include <vector>

#include "bb.h"
#include "token.h"

/*
The `Generator` class is responsible for writing the correctly formatted 
code output for the language. This code generator emits C code, but it 
would be pretty straightforward to implement other language generators. 
*/
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

    // Emits the dotimes loop to the output.
    void emitDoTimes(Token token);

    // Emits a read(<identifier>) to the output
    void emitRead(Token identifier);

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

    // Tracks the indent level for when pretty print mode is enabled
    int m_indentLevel = 1;

    // Flushes a line to the output buffer
    void flushLine(bool startOfLine);

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

    /*
     * The following are helper methods for dealing with formatting when the PRETTY_PRINT option
     * is enabled in `bb.h`.
     * 
     * PRETTY_PRINT has absolutely no practical effect on the generated output, but it makes the
     * output easier to read by adding in extra newlines, spaces, etc., similar to what a human
     * developer might do. This makes debugging the produced output easier.
    */

    // Handles the start of a line for pretty print mode
    inline void pprint_lineStart()
    {
#ifdef PRETTY_PRINT
        if (m_startOfLine)
            for (int i = 0; i < m_indentLevel; i++)
                m_line << "\t";
#endif
    }

    // Handles the start of a line and decrements the indent level tracker
    inline void pprint_lineStartEnd()
    {
#ifdef PRETTY_PRINT
        m_indentLevel--;
        pprint_lineStart();
#endif
    }

    // Adds a line end to the output when pretty print is enabled
    inline void pprint_lineEnd()
    {
#ifdef PRETTY_PRINT
        m_line << "\n";
#endif
    }

    // Handles the end of a line and marks the next line state as being the 
    // start of a line
    inline void pprint_lineEndStart()
    {
#ifdef PRETTY_PRINT
        pprint_lineEnd();
        m_startOfLine = true;
#endif
    }

    // Adds a space to the output when pretty print is enabled
    inline void pprint_space()
    {
#ifdef PRETTY_PRINT
        m_line << " ";
#endif
    }

    // Adds a newline to the file output
    inline void pprint_fileLineEnd()
    {
#ifdef PRETTY_PRINT
        m_file << "\n";
#endif
    }

    // Adds a newline to the file output and sets the start of line state
    // for the next line
    inline void pprint_fileLineEndStart()
    {
#ifdef PRETTY_PRINT
        pprint_fileLineEnd();
        m_startOfLine = true;
#endif
    }

    // Handles the line start in file output when pretty print is enabled
    inline void pprint_fileLineStart() 
    {
#ifdef PRETTY_PRINT
        if (m_startOfLine)
            m_file << "\t";
#endif
    }
};

#endif
