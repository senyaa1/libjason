#include <stdlib.h>
#include "json.h"

void json_free_object(json_object_t* obj)
{
	for(int i = 0; i < obj->elem_cnt; i++)
	{
		json_free_val(&obj->elements[i].value);
		free(obj->elements[i].key);
	}

	free(obj->elements);
	free(obj);
}

void json_free_val(json_value_t* val)
{
	switch(val->type)
	{
		case JSON_STRING:
			free(val->value.str);
			break;
		case JSON_ARRAY:
			json_free_array(val->value.arr);
			break;
		case JSON_OBJECT:
			json_free_object(val->value.obj);
			break;
		default:
			break;
	}
}

void json_free_array(json_array_t* arr)
{
	for(int i = 0; i < arr->length; i++)
		json_free_val(&arr->arr[i]);

	free(arr->arr);
	free(arr);
}

void json_free(json_value_t val)
{
	json_free_val(&val);
}

