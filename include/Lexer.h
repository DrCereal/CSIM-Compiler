#ifndef LEXER_H
#define LEXER_H

#define MAX_TOKENS			1024
#define TOKEN_DATA_SIZE		1024

typedef enum
{
	INVALID_TOKEN = -1,
	BRACE_OPEN,
	BRACE_CLOSE,
	EQUAL,
	IDENTIFIER,
	KEYWORD,
	NUMBER,
	PAREN_OPEN,
	PAREN_CLOSE,
	PERIOD,
	PLUS,
	SEMICOLON,
	STRING,
} TokenType;

typedef struct
{
	TokenType	type;
	int 		num;
	char		data[TOKEN_DATA_SIZE];
} Token;

void DisplayTokens(unsigned int token_amount);
void PrintTokenType(TokenType type);
int LoadTokens();

int AtEOT();
Token GetToken();
Token PeekToken();
void UnGetToken();

#endif
