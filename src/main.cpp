#include <fstream>
#include <iostream>
#include <memory>

#include "generator.h"
#include "lexer.h"
#include "parser.h"

int main(int argc, char* argv[])
{
    // Check that an argument was supplied
    if (argc < 2)
    {
        std::cerr << "You must supply an input file to be compiled." 
            << std::endl;
        return -1;
    }

    //  Check that the supplied input file exists
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    
    if (!inputFile) 
    {
        // The input file does not exist, can't continue
        std::cerr << "Cannot access the input file: " << argv[1] << std::endl;
        return -1;
    }

    // The input file exits, start the compilation process
    auto lexer = std::make_shared<Lexer>(inputFile);
    auto generator = std::make_shared<Generator>("out.c");
    Parser* parser = new Parser(lexer, generator);
    parser->parse();

    // Close the input file
    inputFile.close();

    return 0;
}
