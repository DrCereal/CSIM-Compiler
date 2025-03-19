#include <assert.h>
#include <stdio.h>

#include "File.h"

static FILE* g_file;
static File_Buffer g_buffer = {.start = 0, .end = BUFFER_SIZE, .eof = 0};
static unsigned int g_position = 0;

int OpenFile(const char* file_name)
{
	g_file = fopen(file_name, "r");

	unsigned int read = fread(g_buffer.data, sizeof(char), BUFFER_SIZE, g_file);
	if (read != BUFFER_SIZE) {
		if (ferror(g_file)) {
			printf("OpenFile(): Error reading file!\n");
			return -1;
		}

		g_buffer.end = read;
		g_buffer.eof = 1;
	}

	return g_file != NULL;
}

int AtEOF()
{
	if (g_buffer.eof)
		return g_position >= g_buffer.end;

	return 0;
}

char GetC()
{
	assert(g_file != NULL && "GetC(): No open file!");
	
	if (g_position == g_buffer.end) {
		if (AtEOF())
			return g_buffer.data[g_position];

		// TODO: Read the next section of data.
	}

	return g_buffer.data[g_position++];
}

char PeekC()
{
	assert(g_file != NULL && "PeekC(): No open file!");
	return g_buffer.data[g_position];
}

void UnGetC()
{
	assert(g_position > 0 && "UnGetC(): Trying to move index below zero!");
	g_position--;
}
