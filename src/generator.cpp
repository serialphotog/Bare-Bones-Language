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
}

void Generator::emitProgram(const std::vector<std::string>& identifierMap)
{
    // Start by writing the necessary includes
    m_file << "#include <stdio.h>\n";

#ifdef PRETTY_PRINT
    // The pretty print option is enabled in `bb.h`. Add an extra newline for
    // better readability
    m_file << "\n";
#endif

    // Start the main()
    m_file << "void main() {";

#ifdef PRETTY_PRINT
    m_file << "\n";
    m_startOfLine = true;
#endif

    // Initialize the identifiers
    emitInitializations(identifierMap);

    // Emit the remaining lines of the program
    emitOutput();

    // Finish off the main()
    m_file << "}";

    // Ensure the changes get flushed to disk
    m_file.flush();
}

void Generator::emitOutput()
{
    // Iterate over the lines and emit them to disk
    for (int i=0; i < m_lines.size(); i++)
    {
        m_file << m_lines[i];
    }
}

void Generator::emitInitializations(const std::vector<std::string> identifierMap)
{
    // Iterate over all of the identifiers and declare them in the output
    for (int i=0; i < identifierMap.size(); i++)
    {
#ifdef PRETTY_PRINT
        // The pretty print option is enabled in `bb.h`, add a tab for better
        // readability
        if (m_startOfLine)
        {
            m_file << "\t";
        }
#endif

        m_file << "int " << identifierMap[i] << ";\n";
    }

#ifdef PRETTY_PRINT
    // the pretty print option is enabled, add an extra newline to the output
    m_file << "\n";
#endif
}

Generator::~Generator()
{
    // Close the output file
    m_file.close();
}

void Generator::emitPrint(const std::string& str, const std::vector<std::string>& idents)
{
    std::stringstream ss;
    // Start by pushing the opening/closing `"` and the print string to the output
    ss << "\"";
    ss << str;
    ss << "\"";
    
    // Next, append the identifiers
    for (int i=0; i < idents.size(); i++)
    {
        // The first identifer needs a comma prepended
        if (i == 0)
            ss << ',';

#ifdef PRETTY_PRINT
            ss << ' ';
#endif

        ss << idents[i];

        // Append a comma on all except the last identifier
        if (i < idents.size() - 1)
#ifdef PRETTY_PRINT
            ss << ", ";
#else
            ss << ',';
#endif
    }

    // Emit the output
    emitTight(ss.str().c_str());
}

void Generator::emit(const char* sequence)
{
    // Write a space if this isn't the start of a line
    if (m_startOfLine)
    {
#ifdef PRETTY_PRINT
    // The pretty print option is enable in `bb.h`. Add a \t for easier to 
    // read output
    for (int i=0; i < m_indentLevel; i++)
        m_line << "\t";
#endif
    }

    // Write the output
    m_line << sequence;
    m_startOfLine = false;
}

void Generator::emitTight(const char* sequence)
{
    m_line << sequence;
}

void Generator::emitBlockStart()
{
#ifdef PRETTY_PRINT
    // Pretty print is set, add a space for better readability
    m_line << " ";
#endif

    m_line << "{";

#ifdef PRETTY_PRINT
    // The pretty print option is enabled in `bb.h`, add a newline for easier
    // output debugging
    m_line << "\n";
    m_indentLevel++;
    m_startOfLine = true;

    // Push the line to the vector and clear it for future use
    m_lines.push_back(m_line.str());
    m_line.str("");
    m_line.clear();
#endif
}   

void Generator::emitBlockEnd()
{
#ifdef PRETTY_PRINT
    m_indentLevel--;
    for (int i=0; i < m_indentLevel; i++)
        m_line << "\t";
#endif

    m_line << "}\n";

    // Push the line to the vector and clear it for future use
    m_lines.push_back(m_line.str());
    m_line.str("");
    m_line.clear();
}

void Generator::emitLineEnd()
{
    m_line << ";";

#ifdef PRETTY_PRINT
    // The pretty print option is enabled in `bb.h`, add a newline for easier
    // output debugging
    m_line << "\n";
    m_startOfLine = true;
#endif

    // Push the line to the lines vector and clear it for future use
    m_lines.push_back(m_line.str());
    m_line.str("");
    m_line.clear();
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
    else if (Token::isComparisonOperator(token))
    {
#ifdef PRETTY_PRINT
        // Pretty print mode is enabled, add a space for better readability
        emitTight(" ");
#endif
        if (Token::isKind(token, T_AND))
            emit("&&");
        else if (Token::isKind(token, T_OR))
            emit("||");
        else
            emit(token.lexeme().c_str());

#ifdef PRETTY_PRINT
        // Pretty print mode is enabled, add a space for better readability
        emitTight(" ");
#endif
    }
    else 
    {
        if (Token::isKind(token, T_LPAREN) 
            || Token::isKind(token, T_RPAREN))
        {
            // Keep the output a bit more clean
            emitTight(token.lexeme().c_str());
        }
        else
        {
            // Just write the token as-is
            emitTight(token.lexeme().c_str());
        }
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
#ifdef PRETTY_PRINT
            // Pretty print mode is enabled, add a space for better readability
            emit(" ");
#endif
            break;
        // A few need special treatment, however
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
        // Handle the operators that are identical to those in C
        default:
#ifdef PRETTY_PRINT
            // Add a space after operator when pretty print mode is enabled
            emit(" ");
#endif
           emit(token.lexeme().c_str()); 
#ifdef PRETTY_PRINT
            // Add a space after operator when pretty print mode is enabled
            emit(" ");
#endif
     }
}
