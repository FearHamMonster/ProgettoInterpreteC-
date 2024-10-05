#include <iostream>
#include <string>
#include <stdlib.h>
#include <fstream>

#include "Exceptions.h"
#include "Token.h"
#include "Tokenizer.h"
#include "Parser.h"
#include "Visitor.h"


int main(int argc, char* argv[]) {

    // Command line parsing
    if (argc < 2) {
        std::cerr << "File not found!" << std::endl;
        std::cerr << "Usage: " << argv[0] << " <file_name>" << std::endl;
        return EXIT_FAILURE;
    }

    // Opening input file
    std::ifstream inputFile;
    try {
        inputFile.open(argv[1]);
    }
    catch (std::exception const& exc) {
        std::cerr << "Cannot open " << argv[1] << std::endl;
        std::cerr << exc.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Lexical analysis
    Tokenizer tokenize;
    std::vector<Token> inputTokens;
    try {
        inputTokens = std::move(tokenize(inputFile));
        inputFile.close();
    }
    catch (LexicalError const& le) {
        std::cerr << "Lexical error" << std::endl;
        std::cerr << le.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (std::exception const& exc) {
        std::cerr << "Cannot read from " << argv[1] << std::endl;
        std::cerr << exc.what() << std::endl;
        return EXIT_FAILURE;
    }

    for(int i = 0; i<inputTokens.size(); i++)
    {
        std::cout<<inputTokens[i].tag<<" "<<inputTokens[i].word<<std::endl;
    }

    // Analisi sinttattica
    ExpressionManager manager;
    Program* program = nullptr;
    try {
        Parser parser(manager, inputTokens);
        program = parser();
    }
    catch (ParseError const& pe) {
        std::cerr << "Parse error" << std::endl;
        std::cerr << pe.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (std::exception const& exc) {
        std::cerr << "Unknown error" << std::endl;
        std::cerr << exc.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Valutazione (Analisi semantica)
    try {
        PrintVisitor* p = new PrintVisitor();
        std::cout << "PrintVisitor: \n";
        program->accept(p);
        std::cout << std::endl;
        Environment env(manager);
        EvaluationVisitor* v = new EvaluationVisitor(env);
        std::cout << "\nEvaluationVisitor: \n";
        program->accept(v);
    }
    catch (EvaluationError const& ee) {
        std::cerr << "Errore nella valutazione" << std::endl;
        std::cerr << ee.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (std::exception const& exc) {
        std::cerr << "Errore generico " << std::endl;
        std::cerr << exc.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
