#ifndef JS_READER
#define JS_READER

#include <stddef.h>
#include <stdbool.h>

typedef enum{
	JS_OBJECT,
	JS_ARRAY,
	JS_STRING,
	JS_NUMBER,
	JS_BOOL,
	JS_NULL
}js_type;

typedef struct js_member js_member;
typedef struct js_object js_object;
typedef struct js_array js_array;
typedef struct js_data js_data;

struct js_array{
	size_t count;
	js_data *values;
};

struct js_object{
	size_t count;
	js_member *members;
};

struct js_data{
	js_type type;
	union{
		js_object object;
		js_array array;
		char *string;
		double number;
		bool boolean;
	};
};

struct js_member{
	char *key;
	js_data value;
};

char *js_strndup(const char *src, size_t n);
char *js_load_content(const char *file_path);
js_data js_read_content(const char *content);
void js_free(js_data *data);

#ifdef JS_READER_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// required for C11
char *js_strndup(const char *src, size_t n){
    if(!src) return NULL;

    char *dst = malloc(n + 1);
    if(!dst) return NULL;

    memcpy(dst, src, n);
    dst[n] = '\0';
    return dst;
}

#endif //JS_READER_IMPLEMENTATION
#endif //JS_READER
