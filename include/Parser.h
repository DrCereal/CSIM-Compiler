#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>

// Flags:
// 0 - valid
// 1 - constant

typedef struct
{
	uint8_t	flags;
	int32_t	size;
	int32_t alloc;
	int32_t eval;

	Token* tokens;
} Expression;

// TODO: Collapse these structures into one.
// Flags:
// 0 - valid
// 1 - used

typedef struct
{
	uint8_t		flags;

	Token		type;
	Token		identifier;
	Token		scope;		// This will be the function's name.

	Expression	expr;
} Declaration;

// Flags:
// 0 - valid

typedef struct
{
	uint8_t		flags;
	int32_t		size;
	int32_t		alloc;

	Expression 	expr;
	Token*		tokens;
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

// Flags:
// 0 - valid
// 1 - used

typedef struct
{
	uint8_t		flags;

	Token		name;
	Token		return_type;

	// TODO: args
	
	Compound	compound;
} Function;

void DisplayFunctions();
void ParseBegin();
void ParseCleanup();

#endif
