#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "fs.h"
#include "json.h"

#include "graph.h"

int main(int argc, char** argv)
{
	char* json_text = 0;
	size_t len = read_file(argv[1], &json_text);

	printf("original: %ls\n", json_text);

	json_object_t* json = json_parse(json_text, len);

	render_graph(json, "amogus.png");

	json_free_object(json);
	free(json_text);
	
	return 0;
}
