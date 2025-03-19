#include <stdio.h>

#include "File.h"
#include "Lexer.h"
#include "Parser.h"

int main(int argc, char** argv)
{
	if (argc != 2) {
		printf("invalid usage!\n");
		return -1;
	}

	if (!OpenFile(argv[1])) {
		printf("Failed to open file!\n");
		return -1;
	}

	LoadTokens();
	ParseBegin();

	return 0;
}
