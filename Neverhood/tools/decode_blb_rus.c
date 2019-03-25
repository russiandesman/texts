#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>


#define OFFSETS_MAX 512
#define STRLEN_MAX 512

struct xlat
{
	char in;
	char out;
};

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

char eng[] =     "qwertyuiop%]asdfghjkl$&zxcvbnm@#`";
wchar_t rus[] = L"йцукенгшщзхъфывапролджэячсмитьбюё";

wchar_t xlat_char(char ch)
{
	size_t index = 0;
	wchar_t result;
	char* pos = strchr(eng, ch);
	if ( pos )
	{
		index = pos - eng;
		result = rus[index];

	}
	else
	{
		result = ch;
	}
	return result;
}

void decode_str(const char* s, wchar_t* outbuf)
{
	int i;
	for ( i = 0; i < strlen(s); i++ )
	{
		outbuf[i] = xlat_char(s[i]);
	}
	outbuf[i] = 0;
}

void process_file(char* infile, char* outfile)
{
	char buff[STRLEN_MAX];
	wchar_t decoded_buff[STRLEN_MAX];
	FILE* f_in = fopen(infile, "rb");
	FILE* f_out = fopen(outfile, "wt");
	setlocale(LC_ALL, "en_US.utf8");
	while ( !feof(f_in) )
	{
		fgets(buff, sizeof(buff), f_in);
		decode_str(buff, decoded_buff);
		fputws(decoded_buff, f_out);
	}

	fclose(f_in);
	fclose(f_out);
	
	return;
}

int main(int argc, char **argv)
{
	for ( int i = 1; i < argc; i++ )
	{
		char outfile[FILENAME_MAX];
		sprintf(outfile, "%s.rus", argv[i]);
		printf("processing file %s, output is a %s\n", argv[i], outfile);
		process_file(argv[i], outfile);
	}
	return 0;
}

