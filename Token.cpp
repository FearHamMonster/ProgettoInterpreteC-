#include <sstream>

#include "Token.h"

std::ostream& operator<<(std::ostream& os, const Token& t) {
	std::stringstream tmp;
	tmp << "(" << t.tag << " ; " << t.word << ")";
	os << tmp.str();
	return os;
}

const char* Token::id2word[]{
	"(", ")","{","}","[","]","+", "-", "*", "/", "||", "&&", "==", "!=", "<", "<=", ">", ">=", "!", "=", ";", "NUM", "ID", "if", "else", "do",
	"while", "break", "int", "boolean", "true", "false","print"
};

const int Token::keywordsId[]{Token::IF, Token::ELSE,Token::DO, Token::WHILE, Token::BREAK,
Token::INT, Token::BOOL, Token::TRUE, Token::FALSE, Token::PRINT};
