#include <stdio.h>

#include "json.h"
#include "io.h"

void json_print_obj(json_object_t *obj)
{
	printf(BLUE UNDERLINE "[object]" RESET);

	printf(WHITE " {\n" RESET);
	for(int i = 0; i < obj->elem_cnt; i++)
	{
		printf("\t");
		json_print_str(obj->elements[i].key);
		printf(": ");
		json_print_val(&obj->elements[i].value);
		printf("\n");
	}

	printf(WHITE "}" RESET);
}

void json_print_arr(json_array_t *arr)
{
	printf(BLUE UNDERLINE "[array]" RESET);
	printf(WHITE " [" RESET);

	for(int i = 0; i < arr->length; i++)
	{
		json_print_val(&arr->arr[i]);
		printf("  ");
	}
	printf(WHITE "]" RESET);
}

void json_print_str(json_char_t* str)
{
	printf(GREEN "\"%s\"" RESET, str);
}

void json_print_val(json_value_t *val)
{
	switch(val->type)
	{
		case JSON_OBJECT:
			json_print_obj(val->value.obj);
			break;
		case JSON_ARRAY:
			json_print_arr(val->value.arr);
			break;
		case JSON_STRING:
			json_print_str(val->value.str);
			break;
		case JSON_NUMBER:
			printf(YELLOW "%lg" RESET, val->value.num);
			break;
		case JSON_TRUE:
			printf(BLUE "true" RESET);
			break;
		case JSON_FALSE:
			printf(BLUE "false" RESET);
			break;
		case JSON_NULL:
			printf(BLUE "null" RESET);
			break;
		default:
			printf(RED UNDERLINE "[INVALID]" RESET);
			break;
	}
}

