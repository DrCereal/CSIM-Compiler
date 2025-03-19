#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "Lexer.h"
#include "Parser.h"

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

	if (ret.type == INVALID_TOKEN)
		return ret;

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
static int PrimaryExpression(Token* buffer, int i)
{
	Token t = PeekToken();
	if (t.type != IDENTIFIER && t.type != NUMBER && t.type != STRING)
		return 0;
	GetToken();		// Eat
	
	assert(i < MAX_DECLARATION_TOKENS && "PrimaryExpression(): too many tokens!\n");
	buffer[i] = t;
    
	return 1;
}

static inline int PostfixExpression(Token* buffer, int i)
{
	// TODO: Implement
	return PrimaryExpression(buffer, i);
}

static inline int UnaryExpression(Token* buffer, int i)
{
	// TODO: Implement
	return PostfixExpression(buffer, i);
}

static inline int CastExpression(Token* buffer, int i)
{
	// TODO: Implement
	return UnaryExpression(buffer, i);
}

static inline int MultiplicativeExpression(Token* buffer, int i)
{
	// TODO: Implement
	return CastExpression(buffer, i);
}

static inline int AdditiveExpression(Token* buffer, int i)
{
	// TODO: Implement
	return MultiplicativeExpression(buffer, i);
}

static inline int ShiftExpression(Token* buffer, int i)
{
	// TODO: Implement
	return AdditiveExpression(buffer, i);
}

static inline int RelationalExpression(Token* buffer, int i)
{
	// TODO: Implement
	return ShiftExpression(buffer, i);
}

static inline int EqualityExpression(Token* buffer, int i)
{
	// TODO: Implement
	return RelationalExpression(buffer, i);
}

static inline int AndExpression(Token* buffer, int i)
{
	// TODO: Implement
	return EqualityExpression(buffer, i);
}

static inline int ExclusiveOrExpression(Token* buffer, int i)
{
	// TODO: Implement
	return AndExpression(buffer, i);
}

static inline int InclusiveOrExpression(Token* buffer, int i)
{
	// TODO: Implement
	return ExclusiveOrExpression(buffer, i);
}

static inline int LogicalAndExpression(Token* buffer, int i)
{
	// TODO: Implement
	return InclusiveOrExpression(buffer, i);
}

static inline int LogicalOrExpression(Token* buffer, int i)
{
	// TODO: Implement
	return LogicalAndExpression(buffer, i);
}

static inline int ConditionalExpression(Token* buffer, int i)
{
	// TODO: Implement
	return LogicalOrExpression(buffer, i);
}

static inline int AssignmentExpression(Token* buffer, int i)
{
	// TODO: Other options
	return ConditionalExpression(buffer, i);
}

static inline int Initializer(Token* buffer, int i)
{
	// TODO: Other options
	return AssignmentExpression(buffer, i);
}

static int InitDeclarator(Token* identifier, Token* buffer)
{
	// TODO: Case where it's just a declarator
	int count = 0;
	
	*identifier = Declarator();
	if (identifier->type == INVALID_TOKEN)
		return -1;
	else
		count++;

	Token t = PeekToken();
	if (t.type != EQUAL)
		return count;
	GetToken();		// Eat
	
	count += Initializer(buffer, 0);
	return count;
}

static Declaration FuncDeclaration()
{
	Declaration ret = {.valid = 0};

	ret.type = DeclarationSpecifier();
	if (ret.type.type == INVALID_TOKEN)
		return ret;

	Token buffer[MAX_DECLARATION_TOKENS];
	int grabbed = InitDeclarator(&ret.identifier, buffer);
	if (grabbed <= 0) {
		printf("ERROR: Got useless empty declaration!\n");
		return ret;
	}
	ret.valid = 1;

	for (int i = 0; i < grabbed; i++)
		ret.tokens[i] = buffer[i];

	if (Expect(SEMICOLON))
		GetToken();		// Eat

	return ret;
}

static void CompoundStatement(Compound* compound)
{
	compound->valid = 0;
	compound->declaration_amount = 0;
	compound->statement_amount = 0;

	if (!Expect(BRACE_OPEN))
		return;

	//Declaration dec = FuncDeclaration();
	Declaration dec = {.valid = 1};

	if (dec.valid) {
		// TODO: Add it to list.
		int amount = compound->declaration_amount;
		if (amount < MAX_DECLARATIONS) {
			compound->valid = 1;
			compound->declarations[amount] = dec;
			compound->declaration_amount++;
		} else {
			printf("ERROR: Too many declarations!\n");
		}
	}
}

static Function FunctionDefinition()
{
	Function ret = {.valid = 0};
	
	Token type = DeclarationSpecifier();
	if (type.type != INVALID_TOKEN)
		ret.return_type = type;

	ret.name = Declarator();
	if (ret.name.type != IDENTIFIER)
		return ret;

	// TODO: Declarations.
	
	CompoundStatement(&ret.compound);	
	if (!ret.compound.valid)
		return ret;

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
	} else {
		printf("ERROR: Got no function!\n");
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
