#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <wchar.h>
#include <string.h>
#include <math.h>

#include "fs.h"

typedef enum JSON_VALUE_TYPE
{
	JSON_NONE	= 0,
	JSON_OBJECT	= 1,
	JSON_ARRAY	= 2,
	JSON_NUMBER	= 3,
	JSON_STRING	= 4,
	JSON_TRUE	= 5,
	JSON_FALSE	= 6,
	JSON_NULL	= 7,
} json_value_type_t;

typedef struct json_object json_object_t;
typedef void* json_generic_t;
typedef long double json_number_t;
typedef wchar_t json_char_t;

struct json_object
{
	wchar_t* name;
	json_value_type_t value_type;
	json_generic_t* value;
};

const size_t STR_DEFAULT_ALLOC_SZ = 32;

static inline bool is_whitespace(wchar_t sym)
{
	return sym == L' ' || sym == L'\t' || sym == L'\r' || sym == L'\n';
}


json_number_t json_parse_number(wchar_t* json_text, size_t len, size_t num_start)
{
	size_t actual_start = num_start;
	while(actual_start < len)
	{
		wchar_t sym = json_text[actual_start];
		actual_start++;
		if(!is_whitespace(sym))
		{
			actual_start--;
			break;
		}
	}

	wchar_t *end = 0;
	long double num = wcstold(json_text + actual_start, &end);

	if (errno == ERANGE || isnan(num) || !isfinite(num))
	{
		fprintf(stderr, "error during number parsing!\n");
		return NAN;
	}

	return num;
}


json_char_t* json_parse_string(wchar_t* json_text, size_t len, size_t str_start)
{
	bool str_started = false, str_ended = false;
	int begin_index = 0, end_index = 0;

	size_t str_len = 0, str_allocated = STR_DEFAULT_ALLOC_SZ;
	json_char_t* str = (wchar_t*)calloc(str_allocated, sizeof(wchar_t));

	bool prev_was_escaped_backslash = false;
	for(int i = str_start; i < len - 1; i++)
	{
		if(str_ended) break;

		if(str_len + 1 >= str_allocated)
		{
			str_allocated *= 2;
			str = (wchar_t*)realloc(str, str_allocated);
			fprintf(stderr, "reallocing!\n");
		}

		wchar_t sym = json_text[i];

		if(is_whitespace(sym) && !str_started)
			continue;

		if(!is_whitespace(sym) && !str_started && sym != L'"')
		{
			fprintf(stderr, "Encountered symbols before string start!\n");
			return 0;
		}

		bool escaped = i > 1 && json_text[i - 1] == '\\' && !prev_was_escaped_backslash;
		if (!escaped && sym == L'\\')
		{
			prev_was_escaped_backslash = false;
			continue;
		}

		if (escaped && sym == L'\\')
		{
			prev_was_escaped_backslash = true;
			str[str_len++] = '\\';
			continue;
		}

		prev_was_escaped_backslash = false;
		
		if(escaped)
		{
			switch(sym)
			{
				default:
					fprintf(stderr, "invalid escape sequence! (\\%c)\n", sym);
					return 0;
				case L'r':
					str[str_len++] = L'\r';
					break;
				case L'n':
					str[str_len++] = L'\n';
					break;
				case L't':
					str[str_len++] = L'\t';
					break;
				case L'b':
					str[str_len++] = L'\b';
					break;
				case L'f':
					str[str_len++] = L'\f';
					break;
				case '/':
				case '"':
					str[str_len++] = sym;
					break;

				case 'u':			// parse things such as \u002F (/)
					errno = 0;
					wchar_t hex[5] = { 0 };
					wchar_t *end = 0;

					if(len - i - 1 < 4)
					{
			            		fprintf(stderr, "can't parse unicode code point cause string is out of bounds!\n");
						return 0;
					}

					memcpy(hex, json_text + i + 1, sizeof(wchar_t) * 5);

					wchar_t val = wcstol(json_text + i + 1, &end, 16);

				        if (json_text + i + 1 == end || errno == ERANGE)
					{
			            		fprintf(stderr, "unable to parse unicode code point!\n");
						return 0;
					}

					str[str_len++] = val;
					i += 4;
					break;
			}

			continue;
		}

		switch(sym)
		{
			case '"':
				if(!str_started)
				{
					str_started = true;
					begin_index = i;
				}
				else 
				{
					str_ended = true;
					end_index = i;
				}
				break;

			default:
				str[str_len++] = sym;
		}
	}

	return str;
}

// pass ptr to { (not trimmed)
// return ptr to } and write object data to parent
int json_parse_object(json_object_t* parent, wchar_t* json_text, size_t len, size_t object_start)
{
	for(int i = object_start; i < len; i++)
	{
		wchar_t sym = json_text[i];
		if(sym == ' ' || sym == '\t' || sym == '\r' || sym == '\n')
			continue;

		switch(sym)
		{
		     case '{':
				break;
		     case '}':
				break;
		     case ':':
				break;
		     case ',':
			     	break;
		     case '[':
			     	break;
		     case ']':
			     	break;
		}
	}
}

void json_parse(wchar_t* json_text, size_t len)
{
	json_object_t root = (json_object_t){L"[ROOT]", JSON_OBJECT, 0};

	json_parse_object(&root, json_text, len, 0);
}

int main(int argc, char** argv)
{
	wchar_t* json_text = 0;
	size_t len = read_file(argv[1], &json_text);

	// wchar_t* res = json_parse_string(json_text, len, 2);
	// printf("parsed: %ls\n", res);

	// json_number_t num = json_parse_number(json_text, len, 2);
	// printf("parsed: %Lf\n", num);

	// free(res);
	free(json_text);

	// json_parse(json_text, len);
	
	return 0;
}
