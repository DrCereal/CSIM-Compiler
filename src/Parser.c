#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Lexer.h"
#include "Parser.h"

static void DecAddToken(Declaration* dec, Token t)
{
	if (dec->tokens == NULL) {
		dec->tokens = calloc(64, sizeof(t));
		dec->alloc = 64;
	}

	if (dec->size == dec->alloc) {
		dec->tokens = realloc(dec->tokens, dec->alloc * 2 * sizeof(t));
		dec->alloc *= 2;
	}

	dec->tokens[dec->size++] = t;
}

static void AddDeclaration(Compound* com, Declaration dec)
{
	if (com->declarations == NULL) {
		com->declarations = calloc(32, sizeof(dec));
		com->dec_alloc = 32;
	}

	if (com->dec_size == com->dec_alloc) {
		com->declarations = realloc(com->declarations, com->dec_size * 2 * sizeof(dec));
		com->dec_alloc *= 2;
	}

	com->declarations[com->dec_size++] = dec;
}

static int Expect(TokenType type)
{
	Token t = PeekToken();
	if (t.type != type) {
		printf("ERROR: Expected ");
		PrintTokenType(type);
		printf(" but got ");
		PrintTokenType(t.type);
		printf("!\n");
		return 0;
	}

	return 1;
}

static Token TypeSpecifier()
{
	Token ret = PeekToken();
	if (ret.type != KEYWORD) {
		ret.type = INVALID_TOKEN;
		goto exit;
	}

	if (!strcmp(ret.data, "i32"))
		goto eat;
	else
		ret.type = INVALID_TOKEN;

eat:
	GetToken();
exit:
	return ret;
}

static inline Token DeclarationSpecifier()
{
	return TypeSpecifier();
}

static Token Identifier()
{
	Token ret = GetToken();

	if (ret.type != IDENTIFIER) {
		ret.type = INVALID_TOKEN;
		UnGetToken();
	}

	return ret;
}

// TODO: Other cases for directdeclarators
static Token DirectDeclarator()
{
	Token ret = Identifier();

	if (ret.type == INVALID_TOKEN) {
		printf("not identifier\n");
		return ret;
	}

	Token t = PeekToken();
	if (t.type != PAREN_OPEN)
		return ret;
	GetToken();		// Eat

	// TODO: Is this how I want to handle this? Can we continue?
	if (!Expect(PAREN_CLOSE)) {
		ret.type = INVALID_TOKEN;
		UnGetToken();
		UnGetToken();
		return ret;
	}
	GetToken();		// Eat

	return ret;
}

static inline Token Declarator()
{
	return DirectDeclarator();
}

// TODO: Last case
static int PrimaryExpression(Declaration* dec)
{
	Token t = PeekToken();
	if (t.type != IDENTIFIER && t.type != NUMBER && t.type != STRING)
		return 0;
	GetToken();		// Eat
	
	DecAddToken(dec, t);
	return 1;
}

static inline int PostfixExpression(Declaration* dec)
{
	// TODO: Implement
	return PrimaryExpression(dec);
}

static inline int UnaryExpression(Declaration* dec)
{
	// TODO: Implement
	return PostfixExpression(dec);
}

static inline int CastExpression(Declaration* dec)
{
	// TODO: Implement
	return UnaryExpression(dec);
}

static inline int MultiplicativeExpression(Declaration* dec)
{
	// TODO: Implement
	return CastExpression(dec);
}

static inline int AdditiveExpression(Declaration* dec)
{

	int total = MultiplicativeExpression(dec);

	Token next = PeekToken();
	if (next.type == PLUS || next.type == MINUS) {
		DecAddToken(dec, next);
		GetToken();		// Eat
		total += AdditiveExpression(dec);
	}

	return total;
}

static inline int ShiftExpression(Declaration* dec)
{
	// TODO: Implement
	return AdditiveExpression(dec);
}

static inline int RelationalExpression(Declaration* dec)
{
	// TODO: Implement
	return ShiftExpression(dec);
}

static inline int EqualityExpression(Declaration* dec)
{
	// TODO: Implement
	return RelationalExpression(dec);
}

static inline int AndExpression(Declaration* dec)
{
	// TODO: Implement
	return EqualityExpression(dec);
}

static inline int ExclusiveOrExpression(Declaration* dec)
{
	// TODO: Implement
	return AndExpression(dec);
}

static inline int InclusiveOrExpression(Declaration* dec)
{
	// TODO: Implement
	return ExclusiveOrExpression(dec);
}

static inline int LogicalAndExpression(Declaration* dec)
{
	// TODO: Implement
	return InclusiveOrExpression(dec);
}

static inline int LogicalOrExpression(Declaration* dec)
{
	// TODO: Implement
	return LogicalAndExpression(dec);
}

static inline int ConditionalExpression(Declaration* dec)
{
	// TODO: Implement
	return LogicalOrExpression(dec);
}

static inline int AssignmentExpression(Declaration* dec)
{
	// TODO: Other options
	return ConditionalExpression(dec);
}

static inline int Initializer(Declaration* dec)
{
	// TODO: Other options
	return AssignmentExpression(dec);
}

static int InitDeclarator(Declaration* dec)
{
	// TODO: Case where it's just a declarator
	int count = 0;
	
	dec->identifier = Declarator();
	if (dec->identifier.type == INVALID_TOKEN) 
		return -1; 
	else
		count++;

	Token t = PeekToken();
	if (t.type != EQUAL)
		return count;
	GetToken();		// Eat
	
	count += Initializer(dec);
	return count;
}

static Declaration FuncDeclaration()
{
	Declaration ret = {.flags = 0};

	ret.type = DeclarationSpecifier();
	if (ret.type.type == INVALID_TOKEN)
		return ret;

	int grabbed = InitDeclarator(&ret);
	if (grabbed <= 0) {
		printf("ERROR: Got useless empty declaration!\n");
		return ret;
	}
	ret.flags |= 0x01;

	if (Expect(SEMICOLON))
		GetToken();		// Eat

	return ret;
}

static void CompoundStatement(Compound* compound)
{
	compound->valid = 0;

	if (!Expect(BRACE_OPEN))
		return;
	GetToken();

	compound->valid = 1;

	while (!AtEOT()) {
		Declaration dec = FuncDeclaration();

		if (dec.flags & 0x01) {
			AddDeclaration(compound, dec);
		} else {
			printf("removing tokens...\n");
			GetToken();		// TODO: REMOVE
		}

		// TODO: Get statements.
	}
}

static Function g_functions[128];
static unsigned int g_functions_size = 0;

static void DisplayFunction(Function func)
{
	printf("%s\n", func.name.data);
	printf("return type: %s\n", func.return_type.data);

	Compound com = func.compound;
	if (com.declarations != NULL) {
		printf("declarations: %i\n", com.dec_size);

		for (unsigned int i = 0; i < com.dec_size; i++) {
			Declaration dec = com.declarations[i];
			printf("\nDeclaration:\n");
			DisplayToken(dec.type);
			DisplayToken(dec.identifier);
			DisplayTokens(dec.tokens, dec.size);
		}
	}
	
	printf("statements: %i\n", func.compound.stmt_size);
}

void DisplayFunctions()
{
	printf("\nfunctions:\n");
	for (unsigned int i = 0; i < g_functions_size; i++)
		DisplayFunction(g_functions[i]);
}

static void FreeFunction(Function func)
{
	Compound com = func.compound;

	if (com.declarations != NULL) {
		for (unsigned int i = 0; i < com.dec_size; i++) {
			Declaration dec = com.declarations[i];
		
			if (dec.tokens != NULL)
				free(dec.tokens);
		}

		free(com.declarations);
	}
}

static Function FunctionDefinition()
{
	Function ret = {.valid = 0};
	
	Token type = DeclarationSpecifier();
	if (type.type != INVALID_TOKEN)
		ret.return_type = type;

	ret.name = Declarator();
	if (ret.name.type != IDENTIFIER) {
		printf("Didn't get identifier\n");
		return ret;
	}

	// TODO: Declarations.
	
	CompoundStatement(&ret.compound);	
	if (!ret.compound.valid) {
		printf("Didn't get valid compound statement.\n");
		return ret;
	}

	ret.valid = 1;
	return ret;
}

static void ExternalDeclaration()
{
	if (AtEOT())
		return;

	Function tmp = FunctionDefinition();
	
	if (tmp.valid) {
		// We got a function, cool.
		printf("Got a function! YAY\n");
		g_functions[g_functions_size++] = tmp;
	} else {
		printf("ERROR: Got no function!\n");
		FreeFunction(tmp);
		GetToken();		// Eat
	}

	// TODO: or a declaration 
}

void ParseBegin()
{
	// TODO: We need to speak with Lexer to ensure there are tokens to get
	while (!AtEOT()) {
		ExternalDeclaration();
	}

	printf("Done parsing!\n");
}

void ParseCleanup()
{
	for (unsigned int i = 0; i < g_functions_size; i++)
		FreeFunction(g_functions[i]);
}
