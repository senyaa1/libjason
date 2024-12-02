#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "fs.h"
#include "color.h"
#include "json.h"

#include "graph.h"
#include "serializer.h"
#include "deserializer.h"

int main(int argc, char** argv)
{
	char* json_text = 0;
	size_t len = read_file(argv[1], &json_text);

	printf("original: %ls\n", json_text);

	json_value_t json = json_deserialize(json_text);

	// render_graph(&json, "amogus.png");

	char* serialized_buf = json_serialize(&json);

	printf(RESET "Serialized buf: \n%s\n", serialized_buf);

	json_free(json);
	free(json_text);
	
	free(serialized_buf);
	
	return 0;
}
