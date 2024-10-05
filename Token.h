#ifndef TOKEN_H
#define TOKEN_H

#include <string>

// I token della grammatica delle espressioni numeriche sono:
// - Parentesi aperte e chiuse
// - Operatori +, -, *, /
// - Numeri interi

// Un token � solitamente una coppia (ID, parola), dove:
// - ID � un identificativo univoco attribuito al Token
// - parola � la stringa corrispondente al Token

struct Token {
	// Definisco gli identificativi dei Token
	// constexpr -> costanti note al momento della compilazione
	// static -> costanti "a livello di classe"

	//simboli terminali
	static constexpr int LP = 0;
	static constexpr int RP = 1;
	static constexpr int LEFT_CURLY = 2;
	static constexpr int RIGHT_CURLY = 3;
	static constexpr int LEFT_SQUARE = 4;
	static constexpr int RIGHT_SQUARE = 5;
	static constexpr int ADD = 6;
	static constexpr int MIN = 7;
	static constexpr int MUL = 8;
	static constexpr int DIV = 9;
	static constexpr int OR = 10;
	static constexpr int AND = 11;
	static constexpr int EQ = 12;
	static constexpr int NOT_EQ = 13;
	static constexpr int LESS = 14;
	static constexpr int LESS_EQ = 15;
	static constexpr int MORE = 16;
	static constexpr int MORE_EQ = 17;
	static constexpr int NOT = 18;
    static constexpr int ASSIGN = 19;
    static constexpr int END_STMT = 20;

	//simboli terminali definiti con regex
	static constexpr int NUM = 21;
	static constexpr int ID = 22;


    //keyword riservate
	static constexpr int IF = 23;
	static constexpr int ELSE = 24;
	static constexpr int DO = 25;
	static constexpr int WHILE = 26;
	static constexpr int BREAK = 27;
	static constexpr int INT = 28;
	static constexpr int BOOL = 29;
	static constexpr int TRUE = 30;
	static constexpr int FALSE = 31;
	static constexpr int PRINT = 32;

	// si rende id2word non constexpr, in questo modo può essere indicizzata anche con indici non costanti 
	static const char* id2word[];
	
	//keywords_id contiene gli id numerici dei token keyword 
	static const int keywordsId[];

	static const int keywordsIdSize = 10;

	Token(int t, const char* w) : tag{ t }, word{ w } { }
	Token(int t, std::string w) : tag{ t }, word{ w } { }
	~Token() = default;
	Token(Token const&) = default;
	Token& operator=(Token const&) = default;

	// La coppia (ID, parola) che costituisce il Token
	int tag;
	std::string word;
};




std::ostream& operator<<(std::ostream& os, const Token& t);

#endif
