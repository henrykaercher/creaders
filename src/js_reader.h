#ifndef JS_READER
#define JS_READER

typedef enum{
	JS_OBJECT,
	JS_ARRAY,
	JS_STRING,
	JS_NUMBER,
	JS_BOOL,
	JS_NULL
}js_type;

typedef struct js_data js_data;
typedef struct js_member js_member;
typedef struct js_object js_object;
typedef struct js_array js_array;

struct{
	js_type type;
	union{
		js_object object;
		js_array array;
		char *string;
		double number;
		bool boolean;
	};
}js_data;

struct{
	char *key;
	js_data *values;
} js_member;

struct{
	size_t count;
    js_member *members;
}js_object;

struct{
	size_t count;
	js_data *values;
} js_array;

#ifdef JS_READER_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#endif //JS_READER_IMPLEMENTATION
#endif //JS_READER
