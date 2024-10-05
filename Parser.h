#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <algorithm>

#include "Node.h"
#include "ExpressionManager.h"
#include "Token.h"

class Parser {

public:
    Parser(ExpressionManager& manager, std::vector<Token>& tokenStream)
     : em{ manager }, streamEnd{tokenStream.end()}{
        if(tokenStream.empty())
            throw ParseError("Program has no contents");
        else tokenItr=tokenStream.begin();
     }

    ~Parser() = default;
    Parser(Parser const&) = delete;
    Parser& operator=(Parser const&) = delete;

    //un oggetto p di tipo Parser inizia il parsing chiamando p()
    Program* operator()() {
        Program* p = parseProgram();
        if (tokenItr != streamEnd) {
            throw ParseError("Unexpected end of input");
        }
        return p;
    }



private:
    std::vector<Token>::const_iterator streamEnd;

    //tokenItr is the vectorOfTokens given by the tokenizer as output 
    std::vector<Token>::const_iterator tokenItr;
  
    //The class which supports cleaning of heap allocated memory
    ExpressionManager& em;

    //declaredVars has the job of storing all the identifier names of the program that have been
    //declared. Whenever it encounters a double declaration it throws and exception.
    //The reason behind checking double declaration at parsing time rather then evaluation time
    //lies in the nature of declarations inside loops: while(...){int a} would theoratically
    //declare "a" multiple times, but this behaviour is allowed, so double declarations checking is 
    //done by simply counting how many declarations with the same idName appear in the program
    std::vector<std::string> declaredVars;
    
    Program* parseProgram();
    Block* parseBlock();
    Decls* parseDecls();
    Decl* parseDecl();
    Type* parseType();
    Id* parseId();
    Seq* parseSeq();
    Stmt* parseStmt();
    Expression* parseExpression();
    Expression* parseOr();
    Expression* parseAnd();
    Expression* parseEquality();
    Expression* parseRel();
    Expression* parseLowerPrecedenceBinOp();
    Expression* parseHigerPrecedenceBinOp();
    Expression* parseUnaryOp();
    Expression* parseFactor();


    // Safely skipping token
    void safe_next() {
        if (tokenItr == streamEnd) {
            throw ParseError("Unexpected end of input");
        }
        ++tokenItr;
    }

    //Checking that token is the expected one before skipping it
    void consumeToken(const int tokenId)
    {
        if (tokenItr->tag == tokenId)
            safe_next();
        else 
            throw ParseError{
            std::string("Expecting ").
            append(Token::id2word[tokenId]).
            append(", instead found ").
            append(Token::id2word[tokenItr->tag])};
    }

};

#endif
