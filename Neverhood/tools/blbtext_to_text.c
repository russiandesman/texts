#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define OFFSETS_MAX 512
#define STRLEN_MAX 512

void read_string(FILE* f, char* buff)
{
	int i = 0;
	do
	{
		fread(&buff[i], 1, 1, f);
	} while( buff[i++] != 0 );
}

void process_file(char* infile, char* outfile)
{
	uint32_t pages;
	uint32_t offsets[OFFSETS_MAX];
	FILE* f_in = fopen(infile, "rb");
	FILE* f_out = fopen(outfile, "wt");
	
	fread(&pages, sizeof(pages), 1, f_in);
	printf("processing file: %s\npages count: %d\n", infile, pages);
	
	for ( int i = 0; i < pages; i++ )
	{
		fread(&offsets[i], sizeof(offsets[0]), 1, f_in);
		printf("offset[%d] = %d\n", i, offsets[i]);
	}
	long int text_start = ftell(f_in);
	fprintf(f_out, "pages total: %d\n------------------------------\n", pages -1);
	for ( int i = 0; i < pages - 1 ; i++ )
	{
		char buff[STRLEN_MAX];
		fseek(f_in, text_start + offsets[i], SEEK_SET);
		fprintf(f_out, "------------\npage %d\n------\n", i+1);
		do
		{
			read_string(f_in, buff);
			fprintf(f_out, "%s\n", buff);
		} while (ftell(f_in) < text_start + offsets[i+1]);
	}

	fprintf(f_out, "------------------------------\npages total: %d\n", pages -1);

	fclose(f_in);
	fclose(f_out);
	
	return;
}

int main(int argc, char **argv)
{
	for ( int i = 1; i < argc; i++ )
	{
		char outfile[FILENAME_MAX];
		sprintf(outfile, "%s.processed.txt", argv[i]);
		printf("processing file %s, output is a %s\n", argv[i], outfile);
		process_file(argv[i], outfile);
	}
	return 0;
}

