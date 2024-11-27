#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "fs.h"
#include "json.h"

int main(int argc, char** argv)
{
	wchar_t* json_text = 0;
	size_t len = read_file(argv[1], &json_text);

	printf("original: %ls\n", json_text);

	json_parse(json_text, len);

	// json_value_type_t type = json_parse_tfn(json_text, len, 3);
	// printf("parsed: %d\n", type);

	// wchar_t* res = json_parse_string(json_text, len, 3);
	// printf("parsed: %ls\n", res);

	// json_number_t num = json_parse_number(json_text, len, 2);
	// printf("parsed: %Lf\n", num);

	// free(res);
	free(json_text);

	
	return 0;
}
