#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "File.h"
#include "Lexer.h"

#define CHARACTER_TOKEN(p) \
	GetC(); t.type = (p)

#define MIN(a, b)\
	(a) < (b) ? (a) : (b)

static Token g_tokens[MAX_TOKENS];
static unsigned int g_token_amount = 0;
static unsigned int g_token_index = 0;

static inline void AddToken(Token t)
{
	assert(g_token_amount != MAX_TOKENS && "AddToken(): Exceeded max tokens!");
	g_tokens[g_token_amount++] = t;
}

void DisplayToken(Token t)
{
	TokenType type = t.type;
	printf("type: %i\n", type);
		
	if (type == STRING || type == IDENTIFIER || type == KEYWORD)
		printf("data: %s\n", t.data);
	else if (type == NUMBER)
		printf("number: %i\n", t.num);
}

void DisplayGlobalTokens(unsigned int token_amount)
{
	unsigned int cap = MIN(MAX_TOKENS, token_amount);
	cap = MIN(g_token_amount, cap);

	printf("\nTokens:\n");
	for (unsigned int i = 0; i < cap; i++)
		DisplayToken(g_tokens[i]);
}

void DisplayTokens(Token* tokens, unsigned int amount)
{
	printf("\nTokens:\n");
	for (unsigned int i = 0; i < amount; i++)
		DisplayToken(tokens[i]);
}

static const char* g_type_to_string[] = { 
	"{", "}", "=", "IDENTIFIER", "KEYWORD", "NUMBER", "(", ")", ".", "+",
	";", "STRING"
};

inline void PrintTokenType(TokenType type)
{
	printf("'%s'", g_type_to_string[type]);
}

static Token CheckForKeyword(Token t)
{
	if (!strcmp(t.data, "i32")) 
		t.type = KEYWORD;
	else if (!strcmp(t.data, "import"))
		t.type = KEYWORD;
	else if (!strcmp(t.data, "return"))
		t.type = KEYWORD;

	return t;
}

static Token GetCharacter(char c)
{
	Token t = {.type = INVALID_TOKEN};

	switch (c){
	case '=':
		CHARACTER_TOKEN(EQUAL);
		break;
	case '{':
		CHARACTER_TOKEN(BRACE_OPEN);
		break;
	case '}':
		CHARACTER_TOKEN(BRACE_CLOSE);
		break;
	case '-':
		CHARACTER_TOKEN(MINUS);
		break;
	case '(':
		CHARACTER_TOKEN(PAREN_OPEN);
		break;
	case ')':
		CHARACTER_TOKEN(PAREN_CLOSE);
		break;
	case '.':
		CHARACTER_TOKEN(PERIOD);
		break;
	case '+':
		CHARACTER_TOKEN(PLUS);
		break;
	case ';':
		CHARACTER_TOKEN(SEMICOLON);
		break;
	default:
		GetC();
		printf("Unprocessed character '%c'\n", c);
		break;
	}

	return t;
}

static Token GetIdentifier()
{
	Token t = {.type = IDENTIFIER};
	unsigned int i = 0;
	
	while (!AtEOF()) {
		char c = PeekC();

		if (isalpha(c) || isdigit(c) || c == '_') {
			t.data[i++] = c;
			GetC();		// Eat
		} else {
			break;
		}
	}

	return t;
}

static Token GetNumber()
{
	Token t = {.type = INVALID_TOKEN, .num = 0};
	char tmp[128];
	unsigned int i = 0;

	while (!AtEOF()) {
		char c = PeekC();

		if (isdigit(c)) {
			tmp[i++] = c;
			GetC();		// Eat
		} else {
			tmp[i] = 0;

			t.type = NUMBER;
			t.num = atoi(tmp);
			break;
		}
	}

	return t;
}

static Token GetString()
{
	Token t = {.type = STRING};
	unsigned int i = 0;

	GetC();		// Eat initial '"'

	int reached_EOF = 1;
	while (!AtEOF()) {
		char c = PeekC();

		if (c == '"') {
			reached_EOF = 0;
			GetC();		// Eat
			break;
		}

		assert(i <= TOKEN_DATA_SIZE && "GetString(): Exceeded max token data size!");
		t.data[i++] = c;
		GetC();
	}

	assert(!reached_EOF && "GetString(): Reached EOF, expected '\"'!");

	return t;
}

int LoadTokens()
{
	Token t = {.type = INVALID_TOKEN};

	while (!AtEOF()) {
		char c = PeekC();

		if (isdigit(c)) {
			t = GetNumber();
		} else if (isalpha(c)) {
			t = GetIdentifier();
			t = CheckForKeyword(t);
		} else if (c == '"') {
			t = GetString();
		} else if (isspace(c)) {
			GetC();		// Eat
			continue;
		} else {
			t = GetCharacter(c);
		}
		
		AddToken(t);
		t.type = INVALID_TOKEN;
	}	

	DisplayGlobalTokens(100);

	printf("Done tokenizing!\n");

	return 0;
}

int AtEOT()
{
	return g_token_index == (g_token_amount - 1);
}

Token GetToken()
{
	assert(g_token_amount > 0 && "GetToken(): no tokens to get!");
	assert(g_token_index <= g_token_amount && "GetToken(): index out of bounds!");

	if (AtEOT())
		return g_tokens[g_token_index];
	else
		return g_tokens[g_token_index++];
}

Token PeekToken()
{
	assert(g_token_amount > 0 && "PeekToken(): no tokens to get!");
	assert(g_token_index <= g_token_amount && "PeekToken(): index out of bounds!");

	return g_tokens[g_token_index];
}

void UnGetToken()
{
	assert(g_token_index-- > 0 && "UnGetToken(): no tokens to unget!");	
}
