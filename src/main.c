#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "fs.h"
#include "io.h"
#include "json.h"

#include "graph.h"

#include "serialize.h"

int main(int argc, char** argv)
{
	char* json_text = 0;
	size_t len = read_file(argv[1], &json_text);

	printf("original: %ls\n", json_text);

	json_value_t json = json_parse(json_text, len);

	// render_graph(&json, "amogus.png");
	json_print_val(&json);

	char* serialized_buf = serialize_json(&json);

	printf(RESET "Serialized buf: \n%s\n", serialized_buf);

	json_free(json);
	free(json_text);
	
	free(serialized_buf);
	
	return 0;
}
