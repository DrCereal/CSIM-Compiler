#ifndef PARSER_H
#define PARSER_H

#define MAX_STATEMENT_TOKENS	32
#define MAX_DECLARATION_TOKENS	32
#define MAX_DECLARATIONS		32
#define MAX_STATEMENTS			32

typedef struct
{
	int		valid;
	int		amount;

	Token	type;
	Token	identifier;

	Token 	tokens[MAX_DECLARATION_TOKENS];
} Declaration;

typedef struct
{
	int		valid;
	int		amount;

	Token	tokens[MAX_STATEMENT_TOKENS];
} Statement;

typedef struct
{
	int			valid;
	int			declaration_amount;
	int			statement_amount;

	Declaration	declarations[MAX_DECLARATIONS];
	Statement 	statements[MAX_STATEMENTS];
} Compound;

typedef struct
{
	int			valid;

	Token		name;
	Token		return_type;

	// TODO: args?
	
	Compound	compound;
} Function;

void ParseBegin();

#endif
