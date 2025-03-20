#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Lexer.h"
#include "Parser.h"

static void ExprAddToken(Expression* expr, Token t)
{
	if (expr->tokens == NULL) {
		expr->tokens = calloc(64, sizeof(t));
		expr->alloc = 64;
	}

	if (expr->size == expr->alloc) {
		expr->tokens = realloc(expr->tokens, expr->alloc * 2 * sizeof(t));
		expr->alloc *= 2;
	}

	expr->tokens[expr->size++] = t;
}

static void StmtAddToken(Statement* stmt, Token t)
{
	if (stmt->tokens == NULL) {
		stmt->tokens = calloc(64, sizeof(t));
		stmt->alloc = 64;
	}

	if (stmt->size == stmt->alloc) {
		stmt->tokens = realloc(stmt->tokens, stmt->alloc * 2 * sizeof(t));
		stmt->alloc *= 2;
	}

	stmt->tokens[stmt->size++] = t;
}

static inline void FreeExpr(Expression* expr)
{
	if (expr->tokens != NULL) {
		free(expr->tokens);
		expr->tokens = NULL;
	}
}

static inline void FreeStmt(Statement* stmt)
{
	FreeExpr(&stmt->expr);
	if (stmt->tokens != NULL) {
		free(stmt->tokens);
		stmt->tokens = NULL;
	}
}

static void AddDeclaration(Compound* com, Declaration dec)
{
	if (com->declarations == NULL) {
		com->declarations = calloc(32, sizeof(dec));
		com->dec_alloc = 32;
	}

	if (com->dec_size == com->dec_alloc) {
		com->declarations = realloc(com->declarations, com->dec_alloc * 2 * sizeof(dec));
		com->dec_alloc *= 2;
	}

	com->declarations[com->dec_size++] = dec;
}

static void AddStatement(Compound* com, Statement stmt)
{
	if (com->statements == NULL) {
		com->statements = calloc(32, sizeof(stmt));
		com->stmt_alloc = 32;
	}

	if (com->stmt_size == com->stmt_alloc) {
		com->statements = realloc(com->statements, com->stmt_alloc * 2 * sizeof(stmt));
		com->stmt_alloc *= 2;
	}

	com->statements[com->stmt_size++] = stmt;
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
static int PrimaryExpression(Expression* expr)
{
	Token t = PeekToken();
	if (t.type != IDENTIFIER && t.type != NUMBER && t.type != STRING)
		return 0;
	GetToken();		// Eat
	
	ExprAddToken(expr, t);
	if (t.type != NUMBER) {
		expr->flags &= 0xfd;
		return 0;
	} else {
		return t.num;
	}
}

static inline int PostfixExpression(Expression* expr)
{
	// TODO: Implement
	return PrimaryExpression(expr);
}

static inline int UnaryExpression(Expression* expr)
{
	// TODO: Implement
	return PostfixExpression(expr);
}

static inline int CastExpression(Expression* expr)
{
	// TODO: Implement
	return UnaryExpression(expr);
}

static inline int MultiplicativeExpression(Expression* expr)
{
	// TODO: Implement
	return CastExpression(expr);
}

static int AdditiveExpression(Expression* expr)
{

	int total = MultiplicativeExpression(expr);

	Token next = PeekToken();
	if (next.type == PLUS || next.type == MINUS) {
		ExprAddToken(expr, next);
		GetToken();		// Eat
		total += AdditiveExpression(expr);
	}

	return total;
}

static inline int ShiftExpression(Expression* expr)
{
	// TODO: Implement
	return AdditiveExpression(expr);
}

static inline int RelationalExpression(Expression* expr)
{
	// TODO: Implement
	return ShiftExpression(expr);
}

static inline int EqualityExpression(Expression* expr)
{
	// TODO: Implement
	return RelationalExpression(expr);
}

static inline int AndExpression(Expression* expr)
{
	// TODO: Implement
	return EqualityExpression(expr);
}

static inline int ExclusiveOrExpression(Expression* expr)
{
	// TODO: Implement
	return AndExpression(expr);
}

static inline int InclusiveOrExpression(Expression* expr)
{
	// TODO: Implement
	return ExclusiveOrExpression(expr);
}

static inline int LogicalAndExpression(Expression* expr)
{
	// TODO: Implement
	return InclusiveOrExpression(expr);
}

static inline int LogicalOrExpression(Expression* expr)
{
	// TODO: Implement
	return LogicalAndExpression(expr);
}

static inline int ConditionalExpression(Expression* expr)
{
	// TODO: Implement
	return LogicalOrExpression(expr);
}

static inline int AssignmentExpression(Expression* expr)
{
	// TODO: Other options
	return ConditionalExpression(expr);
}

static inline int Initializer(Expression* expr)
{
	// TODO: Other options
	return AssignmentExpression(expr);
}

// TODO: Review
static int InitDeclarator(Declaration* dec)
{
	// TODO: Case where it's just a declarator
	dec->identifier = Declarator();
	if (dec->identifier.type == INVALID_TOKEN) 
		return -1; 

	Token t = PeekToken();
	if (t.type != EQUAL)
		return 0;
	GetToken();		// Eat
	
	return Initializer(&dec->expr);
}

static Declaration FuncDeclaration()
{
	Declaration ret = {.flags = 0x02};

	ret.type = DeclarationSpecifier();
	if (ret.type.type == INVALID_TOKEN)
		return ret;

	ret.expr.eval = InitDeclarator(&ret);
	if (ret.expr.size == 0) {
		printf("ERROR: Got useless empty declaration!\n");
		return ret;
	}
	ret.flags |= 0x01;

	if ((ret.expr.flags & 0x02) && ret.expr.tokens != NULL)
		FreeExpr(&ret.expr);

	if (Expect(SEMICOLON))
		GetToken();		// Eat

	return ret;
}

static int FuncExpression(Expression* expr)
{
	// TODO: Implement features
	return AssignmentExpression(expr);
}

static Statement JumpStatement()
{
	// TODO: Implement features.
	Statement stmt = {.flags = 0};
	
	Token next = PeekToken();
	if (next.type != KEYWORD || strcmp(next.data, "return"))
		return stmt;

	StmtAddToken(&stmt, next);
	GetToken();		// Eat

	stmt.flags |= 0x01;

	stmt.expr.eval = FuncExpression(&stmt.expr);
	if ((stmt.expr.flags & 0x2) && stmt.expr.tokens != NULL)
		FreeExpr(&stmt.expr);

	if (Expect(SEMICOLON))
		GetToken();		// Eat

	return stmt;
}

static Statement FuncStatement()
{
	// TODO: Implement other features
	return JumpStatement();
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
		if (dec.flags & 0x01)
			AddDeclaration(compound, dec);
		else
			FreeExpr(&dec.expr);

		Statement stmt = FuncStatement();
		if (stmt.flags & 0x01)
			AddStatement(compound, stmt);
		else
			FreeStmt(&stmt);

		if (!(stmt.flags & 0x01) && !(dec.flags & 0x01))
			return;
	}
}

static Function g_functions[128];
static unsigned int g_functions_size = 0;

static void DisplayDeclaration(Declaration dec)
{
	printf("\nDeclaration:\n");
	DisplayToken(dec.type);
	DisplayToken(dec.identifier);
	
	if (dec.expr.tokens != NULL)
		DisplayTokens(dec.expr.tokens, dec.expr.size);
	else
		printf("eval: %i\n", dec.expr.eval);
}

static void DisplayStatement(Statement stmt)
{
	printf("\nStatement:\n");
	
	if (stmt.tokens != NULL)
		DisplayTokens(stmt.tokens, stmt.size);

	if (stmt.expr.tokens != NULL)
		DisplayTokens(stmt.expr.tokens, stmt.expr.size);
	else
		printf("eval: %i\n", stmt.expr.eval);
}

static void DisplayFunction(Function func)
{
	printf("%s\n", func.name.data);
	printf("return type: %s\n", func.return_type.data);

	Compound com = func.compound;
	if (com.declarations != NULL) {
		printf("declarations: %i\n", com.dec_size);

		for (unsigned int i = 0; i < com.dec_size; i++)
			DisplayDeclaration(com.declarations[i]);
	}

	if (com.statements != NULL) {
		printf("statements: %i\n", func.compound.stmt_size);

		for (unsigned int i = 0; i < com.stmt_size; i++)
			DisplayStatement(com.statements[i]);
	}
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
		for (unsigned int i = 0; i < com.dec_size; i++)
			FreeExpr(&com.declarations[i].expr);

		free(com.declarations);
	}

	if (com.statements != NULL) {
		for (unsigned int i = 0; i < com.stmt_size; i++)
			FreeStmt(&com.statements[i]);

		free(com.statements);
	}
}

static Function FunctionDefinition()
{
	Function ret = {.flags = 0};
	
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

	ret.flags |= 0x01;
	return ret;
}

static void ExternalDeclaration()
{
	if (AtEOT())
		return;

	Function tmp = FunctionDefinition();
	
	if (tmp.flags & 0x01) {
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
