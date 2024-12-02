#pragma once

#include <string.h>
#include "json.h"

typedef struct buf_writer
{
	char* buf;
	size_t buf_len;
	size_t cursor;
	size_t indent;
} buf_writer_t;


void bufcpy(buf_writer_t* writer, const char* string);
void bufncpy(buf_writer_t* writer, const char* string, size_t len);

void serialize_val(buf_writer_t* writer, json_value_t* val);
void serialize_obj(buf_writer_t* writer, json_object_t* obj);
void serialize_arr(buf_writer_t* writer, json_array_t* arr);

char* serialize_json(json_value_t* val);
