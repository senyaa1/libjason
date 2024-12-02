#include <stdio.h>
#include <string.h>

#include "json.h"
#include "io.h"
#include "serialize.h"

#define SERIALIZED_ANSI_COLORED

static void buf_increase(buf_writer_t* writer)
{
	writer->buf_len *= 2;
	writer->buf = (char*)realloc(writer->buf, writer->buf_len * sizeof(char));
}

static void buf_maybe_increase(buf_writer_t* writer, size_t n)
{
	while(*writer->cursor + n > writer->buf_len - 1)
		buf_increase(writer);
}


void bufcpy(buf_writer_t* writer, const char* string)
{
	size_t len = strlen(string);
	buf_maybe_increase(writer, len);

	memcpy(writer->buf + *writer->cursor, string, len);
	*writer->cursor += len;
}

void bufncpy(buf_writer_t* writer, const char* string, size_t len)
{
	buf_maybe_increase(writer, len);
	memcpy(writer->buf + *writer->cursor, string, len);
	*writer->cursor += len;
}

void bufindent(buf_writer_t* writer)
{
	for(int i = 0; i < writer->indent; i++)		// TODO: fix this afwul shit
		bufcpy(writer, "\t");
}

static void serialize_str(buf_writer_t* writer, json_char_t* str)
{
	// TODO: copy escaped string
	
	char* escaped_str = 0;
	#ifdef SERIALIZED_ANSI_COLORED
	asprintf(&escaped_str, GREEN "\"%s\"" RESET, str);
	#else
	asprintf(&escaped_str, "\"%s\"", str);
	#endif 

	bufcpy(writer, escaped_str);

	free(escaped_str);
}

static void serialize_num(buf_writer_t* writer, json_number_t num)
{
	static char num_str[256] = {0};
	#ifdef SERIALIZED_ANSI_COLORED
	sprintf(num_str, YELLOW "%g" RESET, num);
	#else 
	sprintf(num_str, "%g", num);
	#endif
	bufcpy(writer, num_str);
}


void serialize_obj(buf_writer_t* writer, json_object_t* obj)
{
	bufcpy(writer, "{\n");
	writer->indent++;

	for(int i = 0; i < obj->elem_cnt; i++)
	{
		bufindent(writer);
		serialize_str(writer, obj->elements[i].key);
		bufcpy(writer, ": ");
		serialize_val(writer, &obj->elements[i].value);
		if(i != obj->elem_cnt - 1) bufcpy(writer, ",");

		bufcpy(writer, "\n");
	}

	writer->indent--;
	bufindent(writer);
	bufcpy(writer, "}");
}

void serialize_arr(buf_writer_t* writer, json_array_t* arr)
{
	bufcpy(writer, "[\n");
	writer->indent++;

	for(int i = 0; i < arr->length; i++)
	{
		bufindent(writer);
		serialize_val(writer, &arr->arr[i]);
		if(i != arr->length - 1) bufcpy(writer, ",");
		bufcpy(writer, "\n");
	}

	writer->indent--;
	bufindent(writer);
	bufcpy(writer, "]");
}


void serialize_val(buf_writer_t* writer, json_value_t* val)
{

	switch(val->type)
	{
		case JSON_OBJECT:
			serialize_obj(writer, val->value.obj);
			break;
		case JSON_ARRAY:
			serialize_arr(writer, val->value.arr);
			break;
		case JSON_STRING:
			serialize_str(writer, val->value.str);
			break;
		case JSON_NUMBER:
			serialize_num(writer, val->value.num);
			break;
		case JSON_TRUE:
			bufcpy(writer, "true");
			break;
		case JSON_FALSE:
			bufcpy(writer, "false");
			break;
		case JSON_NULL:
			bufcpy(writer, "null");
			break;
		default:
			fprintf(stderr, RED "Can't serialize value of type %d\n" RESET, val->type);
			return;
	}

}


char* serialize_json(json_value_t* val)
{
	buf_writer_t* writer = (buf_writer_t*)calloc(1, sizeof(buf_writer_t));

	writer->buf_len = SERIALIZED_BUF_DEFAULT_ALLOC_SZ;

	char* buf = (char*)calloc(writer->buf_len, sizeof(char));
	size_t ptr = 0;

	writer->cursor = &ptr;
	writer->buf = buf;

	switch(val->type)
	{
		case JSON_OBJECT:
		case JSON_ARRAY:
			serialize_val(writer, val);
			break;
		default:
			fprintf(stderr, RED "Only objects and array are allowed to be top-level components\n" RESET);
			return 0;
	}

	free(writer);

	return buf;
}
