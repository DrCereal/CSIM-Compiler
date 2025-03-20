#ifndef FILE_H
#define FILE_H

#define BUFFER_SIZE	4096

typedef struct {
	char data[BUFFER_SIZE];
	unsigned int start;
	unsigned int end;
	unsigned int eof;
} File_Buffer;

int AtEOF();
int OpenFile(const char* file_name);
void FileCleanup();
char GetC();
char PeekC();
void UnGetC();

#endif
