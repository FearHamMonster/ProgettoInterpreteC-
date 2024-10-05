#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <vector>
#include <fstream>

#include "Token.h"

class Tokenizer {

public:
	Tokenizer() = default;
	~Tokenizer() = default;
	Tokenizer(Token const&) = delete;
	Token& operator=(Token const&) = delete;

	std::vector<Token> operator()(std::ifstream& inputFile) {
		std::vector<Token> inputTokens;
		tokenizeInputFile(inputFile, inputTokens);
		return inputTokens;
	}

private:
	//this function is used to differentiate between keywords and simple identifiers
    bool isKeyword(std::string stream,  int& token_id);

	void tokenizeInputFile(std::ifstream& inputFile, std::vector<Token>& inputTokens);

};

#endif

