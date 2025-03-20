#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>

#define MAX_STATEMENT_TOKENS	32
#define MAX_DECLARATION_TOKENS	32
#define MAX_DECLARATIONS		32
#define MAX_STATEMENTS			32

// Flags:
// 0 - valid
// 1 - constant

typedef struct
{
	uint8_t	flags;
	int32_t	size;
	int32_t	alloc;
	int32_t	partial_eval;

	Token	type;
	Token	identifier;

	Token* 	tokens;
} Declaration;

typedef struct
{
	char	valid;
	int		size;
	int		alloc;

	Token*	tokens;
} Statement;

typedef struct
{
	char			valid;
	unsigned int	dec_size;
	unsigned int	stmt_size;
	unsigned int	dec_alloc;
	unsigned int	stmt_alloc;

	Declaration*	declarations;
	Statement*	 	statements;
} Compound;

typedef struct
{
	char		valid;

	Token		name;
	Token		return_type;

	// TODO: args?
	
	Compound	compound;
} Function;

void DisplayFunctions();
void ParseBegin();
void ParseCleanup();

#endif
