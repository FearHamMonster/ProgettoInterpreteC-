#include <string>
#include <sstream>

#include "Tokenizer.h"
#include "Exceptions.h"

bool Tokenizer::isKeyword(std::string word, int& token_id)
{

    for(int i = 0; i<Token::keywordsIdSize; i++)
    {
        int keyword_id  = Token::keywordsId[i];
        if(std::string(Token::id2word[keyword_id]) == word)
        {
            token_id = keyword_id;
            return true;
        }
    }
  return false;
}



void Tokenizer::tokenizeInputFile(std::ifstream& inputFile,
	std::vector<Token>& inputTokens) {

	char ch;
	ch = inputFile.get();

	while (!inputFile.eof()) {
		if (std::isspace(ch)) {
			// this step ensures that white spaces are ignored
			ch = inputFile.get();
			continue;
		}
		if (ch == '(') {
			inputTokens.push_back(Token{ Token::LP, Token::id2word[Token::LP] });
		}
		else if (ch == ')') {
			inputTokens.push_back(Token{ Token::RP, Token::id2word[Token::RP] });
		}
		else if (ch == '{') {
			inputTokens.push_back(Token{ Token::LEFT_CURLY, Token::id2word[Token::LEFT_CURLY] });
		}
		else if (ch == '}') {
			inputTokens.push_back(Token{ Token::RIGHT_CURLY, Token::id2word[Token::RIGHT_CURLY] });
		}
		else if (ch == '[') {
			inputTokens.push_back(Token{ Token::LEFT_SQUARE, Token::id2word[Token::LEFT_SQUARE] });
		}
		else if (ch == ']') {
			inputTokens.push_back(Token{ Token::RIGHT_SQUARE, Token::id2word[Token::RIGHT_SQUARE] });
		}
		else if (ch == '+') {
			inputTokens.push_back(Token{ Token::ADD, Token::id2word[Token::ADD] });
		}
		else if (ch == '-') {
			inputTokens.push_back(Token{ Token::MIN, Token::id2word[Token::MIN] });
		}
		else if (ch == '*') {
			inputTokens.push_back(Token{ Token::MUL, Token::id2word[Token::MUL] });
		}
		else if (ch == '/') {
			inputTokens.push_back(Token{ Token::DIV, Token::id2word[Token::DIV] });
		}

		else if (ch == '|') {
            ch = inputFile.get();
			if(ch == '|')
                inputTokens.push_back(Token{ Token::OR, Token::id2word[Token::OR] });
            else
                throw LexicalError("Errore lessicale sul simbolo: |");
		}

		else if (ch == '&') {
            ch = inputFile.get();
			if(ch == '&')
                inputTokens.push_back(Token{ Token::AND, Token::id2word[Token::AND] });
            else
                throw LexicalError("Errore lessicale sul simbolo: &");
		}

		else if (ch == '!') {
			ch = inputFile.peek(); //peek returns the current symbol, without extracting it
			if(ch == '=')
            {
				inputTokens.push_back(Token{ Token::NOT_EQ, Token::id2word[Token::NOT_EQ] });
				inputFile.get(); //'=' is extracted
			}
			else
                inputTokens.push_back(Token{ Token::NOT, Token::id2word[Token::NOT] });
		}

		else if (ch == '<') {
			ch = inputFile.peek();
			if(ch == '=')
			{
				inputTokens.push_back(Token{ Token::LESS_EQ, Token::id2word[Token::LESS_EQ] });
				inputFile.get(); 
			}
			else
                inputTokens.push_back(Token{ Token::LESS, Token::id2word[Token::LESS] });
		}

		else if (ch == '>') {
			ch = inputFile.peek();
			if(ch == '=')
            {
				inputTokens.push_back(Token{ Token::MORE_EQ, Token::id2word[Token::MORE_EQ] });
				inputFile.get(); 
			}    
            else
                inputTokens.push_back(Token{ Token::MORE, Token::id2word[Token::MORE] });
		}

		else if (ch == '=') {
			ch = inputFile.peek();
			if(ch == '=')
            {
				inputTokens.push_back(Token{ Token::EQ, Token::id2word[Token::EQ] });
				inputFile.get(); 
			}
			else
                inputTokens.push_back(Token{ Token::ASSIGN, Token::id2word[Token::ASSIGN] });
		}

		else if (ch == ';') {
			inputTokens.push_back(Token{ Token::END_STMT, Token::id2word[Token::END_STMT] });
		}

		//tokenizing of either identifier or keyword 
		else if (std::isalpha(ch)){

            //
            std::stringstream word{};
			word << ch;
            while(std::isalnum(ch))
            {
                ch = inputFile.peek();

                if(!std::isalnum(ch))
					break;

                word << ch;
				inputFile.get();
            }

             int token_id = 0;

			//token_id is passed as a reference, and ultimately is updated with the correct tokenid
            if(isKeyword(word.str(), token_id))
            {
            	inputTokens.push_back(Token{ token_id, Token::id2word[token_id] });
			}
            else
                inputTokens.push_back(Token{ Token::ID, word.str() });

        }

		//Tokenizing of numeric constant
		else if (std::isdigit(ch)) {
			std::stringstream tmp{};
			tmp << ch;
			do {
				ch = inputFile.get();
				if (std::isdigit(ch)) tmp << ch;
			} while (std::isdigit(ch));
			inputTokens.push_back(Token{ Token::NUM, tmp.str() });
			continue;
		}
		else {
			//if every other symbol is found a lexical error is thrown
			std::stringstream tmp{};
			tmp << "Errore lessicale sul simbolo: " << ch;
			throw LexicalError(tmp.str());
		}
		ch = inputFile.get();
	}
}