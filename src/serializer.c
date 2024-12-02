#include <stdio.h>
#include <string.h>

#include "json.h"
#include "io.h"
#include "serializer.h"

static void buf_increase(buf_writer_t* writer)
{
	writer->buf_len *= 2;
	writer->buf = (char*)realloc(writer->buf, writer->buf_len * sizeof(char));
}

static void buf_maybe_increase(buf_writer_t* writer, size_t n)
{
	while(writer->cursor + n >= writer->buf_len - 1)
		buf_increase(writer);
}


void bufcpy(buf_writer_t* writer, const char* string)
{
	size_t len = strlen(string);
	buf_maybe_increase(writer, len);

	memcpy(writer->buf + writer->cursor, string, len);
	writer->cursor += len;
}

void bufncpy(buf_writer_t* writer, const char* string, size_t len)
{
	buf_maybe_increase(writer, len);
	memcpy(writer->buf + writer->cursor, string, len);
	writer->cursor += len;
}

void bufindent(buf_writer_t* writer)
{
	for(int i = 0; i < writer->indent; i++)	
		bufcpy(writer, "\t");
}

static void serialize_str(buf_writer_t* writer, json_char_t* str)
{
	size_t old_len = strlen(str);
	size_t len = 1, cur_alloc = old_len + 3;

	json_char_t* escaped_str = (json_char_t*)calloc(cur_alloc, sizeof(json_char_t));

	escaped_str[0] = '\"';
	for(int i = 0; i < old_len; i++)
	{
		if(len >= cur_alloc - 3)
		{
			cur_alloc *= 2;
			escaped_str = (json_char_t*)realloc(escaped_str, cur_alloc * sizeof(json_char_t));
		}

		char sym = str[i];

		switch(sym)
		{
			case '\r':
				escaped_str[len++] = '\\';
				escaped_str[len++] = 'r';
				break;
			case '\n':
				escaped_str[len++] = '\\';
				escaped_str[len++] = 'n';
				break;
			case '\t':
				escaped_str[len++] = '\\';
				escaped_str[len++] = 't';
				break;
			case '\b':
				escaped_str[len++] = '\\';
				escaped_str[len++] = 'b';
				break;
			case '\f':
				escaped_str[len++] = '\\';
				escaped_str[len++] = 'f';
				break;
			case '\\':
				escaped_str[len++] = '\\';
				escaped_str[len++] = '\\';
				break;
			case '\"':
				escaped_str[len++] = '\\';
				escaped_str[len++] = '"';
				break;
			default:
				escaped_str[len++] = sym;
				break;
		}
	}

	escaped_str[len++] = '\"';
	escaped_str[len] = '\0';

	bufcpy(writer, escaped_str);

	free(escaped_str);
}

static void serialize_num(buf_writer_t* writer, json_number_t num)
{
	static char num_str[256] = {0};
	sprintf(num_str, "%g", num);
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
	buf_writer_t writer = { 0 };

	writer.buf_len = SERIALIZED_BUF_DEFAULT_ALLOC_SZ;
	writer.buf = (char*)calloc(writer.buf_len, sizeof(char));

	switch(val->type)
	{
		case JSON_OBJECT:
		case JSON_ARRAY:
			serialize_val(&writer, val);
			break;
		default:
			fprintf(stderr, RED "Only objects and array are allowed to be top-level components\n" RESET);
			return 0;
	}

	writer.buf = (char*)realloc(writer.buf, (writer.cursor + 1) * sizeof(char));

	return writer.buf;
}
