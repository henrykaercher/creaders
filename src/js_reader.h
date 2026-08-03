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

typedef enum{
	JS_OK,
	JS_ERR_PARSE,
	JS_ERR_MEMORY
}js_result;

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
	}u;
};

struct js_member{
	char *key;
	js_data value;
};

typedef struct{
    const char *begin;
    const char *cur;
    size_t line;
    size_t column;
}parser_t;

char *js_strndup(const char *src, size_t n);
char *js_load_content(const char *file_path);
js_result json_parse(const char *path, js_data **out);

void skip_spaces(parser_t *p);
js_data parse_value(parser_t *p);
js_data parse_object(parser_t *p);
js_data parse_array(parser_t *p);
js_data parse_string(parser_t *p);
js_data parse_number(parser_t *p);
js_data parse_bool(parser_t *p);
js_data parse_null(parser_t *p);

//helpers
js_data *js_get_object(js_data *obj, const char *key);
js_data *js_get_array(js_data *arr, size_t index);
const char *js_as_string(js_data *data);
double js_as_number(js_data *data, double default_val);
bool js_as_bool(js_data *data, bool default_val);

void js_cleanup_internals(js_data *data);
void js_free(js_data *data);

#ifdef JS_READER_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// required for C11
char *js_strndup(const char *src, size_t n){
    if(!src) return NULL;

    char *dst = malloc(n + 1);
    if(!dst) return NULL;

    memcpy(dst, src, n);
    dst[n] = '\0';
    return dst;
}

char *js_load_content(const char *file_path){
	FILE* js_file = fopen(file_path, "rb");

	if(!js_file){
        perror("fopen");
		return NULL;
	}

	fseek(js_file, 0, SEEK_END);
    long file_size = ftell(js_file);
	rewind(js_file);

	char *content = malloc(file_size + 1);
	if(!content){
		fclose(js_file);
		return NULL;
	}

	size_t bytes_read = fread(content, 1, file_size, js_file);
	content[bytes_read] = '\0';

	fclose(js_file);
	return content;
}

js_result json_parse(const char *path, js_data **out){
    if(!path || !out) return JS_ERR_PARSE;

    char *content = js_load_content(path);
    if(!content) return JS_ERR_MEMORY; 

    parser_t p = { .begin = content, .cur = content, .line = 1, .column = 1 };
    
    skip_spaces(&p);
    js_data root = parse_value(&p);
    *out = malloc(sizeof(js_data));
    if(!*out){
		free(content);
		js_cleanup_internals(&root);
		return JS_ERR_MEMORY;
	}

    **out = root;
	skip_spaces(&p);

	bool has_trailing_garbage = (*p.cur != '\0');
    free(content);

    if(has_trailing_garbage){
		js_free(*out);
		*out = NULL;
		return JS_ERR_PARSE;
	}

    return JS_OK;
}

void skip_spaces(parser_t *p){
	while(*p->cur != '\0' && isspace((unsigned char)*p->cur)){
		if(*p->cur == '\n'){
			p->line++;
			p->column = 1;
			p->cur++;
		}
		else{
			p->cur++;
			p->column++;
		}
	}
}

js_data parse_value(parser_t *p){
	skip_spaces(p);

	//pointers are really something...
	switch(*p->cur){
		case '{': return parse_object(p);
		case '[': return parse_array(p);
		case '"': return parse_string(p);
		case 't': return parse_bool(p);
		case 'f': return parse_bool(p);
		case 'n': return parse_null(p);
		default: 
			if(*p->cur == '-' || isdigit((unsigned char)*p->cur)) return parse_number(p);
			break;
	}

	js_data err = { .type = JS_NULL };
	return err;
}

js_data parse_bool(parser_t *p){
	js_data val = { .type = JS_BOOL, .u.boolean = false };

	if(strncmp(p->cur, "true", 4) == 0){
		val.u.boolean = true;
		p->cur += 4;
		p->column += 4;
	}
	else if(strncmp(p->cur, "false", 5) == 0){
		val.u.boolean = false;
		p->cur += 5;
		p->column += 5;
	}
	else{
		val.type = JS_NULL;
		p->cur++;
        p->column++;
	}

	return val;
}

js_data parse_null(parser_t *p){
    js_data val = { .type = JS_NULL };

    if(strncmp(p->cur, "null", 4) == 0){
        p->cur += 4;
        p->column += 4;
    }
	else{
		p->cur++;
        p->column++;
	}
    return val;
}

js_data parse_string(parser_t *p){
	js_data val = { 
		.type = JS_STRING,
		.u.string = NULL
	};

	if(*p->cur != '"') return val;

	p->cur++;
	p->column++;

	const char *start = p->cur;
	while(*p->cur != '\0'){
		if(*p->cur == '\\'){
			p->cur++;
			p->column++;
			if(*p->cur != '\0'){
				p->cur++;
				p->column++;
			}
		}
		else if(*p->cur == '"'){
			break;
		}
		else{
			p->cur++;
			p->column++;
		}
	}

	size_t length = (size_t)(p->cur - start);
	val.u.string = js_strndup(start, length);

	if(*p->cur == '"'){
		p->cur++;
		p->column++;
	}

	return val;
}

js_data parse_number(parser_t *p){
	js_data val = { 
		.type = JS_NUMBER,
		.u.number = 0.0
	};

	char *endptr = NULL;
	val.u.number = strtod(p->cur, &endptr);

	if(endptr != p->cur){
		p->column += (size_t)(endptr - p->cur);
		p->cur = endptr;
	}
	else{
		printf("No valid number at line: %zu, column %zu ('%s')\n", p->line, p->column, p->cur);
	}

	return val;
}

js_data parse_array(parser_t *p){
	js_data val = { .type = JS_ARRAY, .u.array = { .count = 0, .values = NULL } };
	
	p->cur++;
	p->column++;

	skip_spaces(p);
	if(*p->cur == ']'){
		p->cur++;
		p->column++;
		return val;
	}

	size_t capacity = 4;
	val.u.array.values = malloc(capacity * sizeof(js_data));
	if(!val.u.array.values){
        val.u.array.count = 0;
        return val;
    }

	while(*p->cur != '\0'){
		js_data elem = parse_value(p);

		if(val.u.array.count >= capacity){
			capacity *= 2;
			void *tmp = realloc(val.u.array.values, capacity * sizeof(js_data));
			if(!tmp){
				js_cleanup_internals(&elem);
				break; 
			}
			val.u.array.values = tmp;
		}
		val.u.array.values[val.u.array.count++] = elem;

		skip_spaces(p);
        if(*p->cur == ','){
            p->cur++;
            p->column++;
        }
		else if(*p->cur == ']'){
            p->cur++;
            p->column++;
            break;
        }
		else break;
	}

    return val;
}

js_data parse_object(parser_t *p){
    js_data val = { .type = JS_OBJECT, .u.object = { .count = 0, .members = NULL } };
    
    p->cur++;
    p->column++;

    skip_spaces(p);
    if(*p->cur == '}'){
        p->cur++;
        p->column++;
        return val;
    }

    size_t capacity = 4;
    val.u.object.members = malloc(capacity * sizeof(js_member));
	if(!val.u.object.members){
        val.u.object.count = 0;
        return val;
    }

    while(*p->cur != '\0'){
        skip_spaces(p);

        if(*p->cur != '"') break;

        js_data key_data = parse_string(p);
        skip_spaces(p);

        if(*p->cur != ':'){
            free(key_data.u.string);
            break;
        }
        p->cur++;
        p->column++;

        js_data field_val = parse_value(p);

        if(val.u.object.count >= capacity){
            capacity *= 2;
           	void *tmp = realloc(val.u.object.members, capacity * sizeof(js_member));
			if(!tmp){
				free(key_data.u.string);
                js_cleanup_internals(&field_val);
				break; 
			}
			val.u.object.members = tmp;
		}

        val.u.object.members[val.u.object.count].key = key_data.u.string;
        val.u.object.members[val.u.object.count].value = field_val;
        val.u.object.count++;

        skip_spaces(p);
        if(*p->cur == ','){
            p->cur++;
            p->column++;
        }
		else if(*p->cur == '}'){
            p->cur++;
            p->column++;
            break;
        }
		else break;
    }

    return val;
}

void js_cleanup_internals(js_data *data){
    if(!data) return;

    switch(data->type){
        case JS_STRING:
            free(data->u.string);
            break;

        case JS_OBJECT:
            for(size_t i = 0; i < data->u.object.count; i++){
                free(data->u.object.members[i].key);
                js_cleanup_internals(&data->u.object.members[i].value);
            }
            free(data->u.object.members);
            break;

        case JS_ARRAY:
            for(size_t i = 0; i < data->u.array.count; i++){
                js_cleanup_internals(&data->u.array.values[i]);
            }
            free(data->u.array.values);
            break;

        default:
            break;
    }
}

void js_free(js_data *data){
    if(!data) return;
	js_cleanup_internals(data);
    free(data);
}

js_data *js_get_object(js_data *obj, const char *key){
    if(!obj || obj->type != JS_OBJECT || !key) return NULL;

    for(size_t i = 0; i < obj->u.object.count; i++){
        if(strcmp(obj->u.object.members[i].key, key) == 0){
            return &obj->u.object.members[i].value;
        }
    }
    return NULL;
}

js_data *js_get_array(js_data *arr, size_t index){
    if(!arr || arr->type != JS_ARRAY) return NULL;

    if(index < arr->u.array.count){
        return &arr->u.array.values[index];
    }
    return NULL;
}

const char *js_as_string(js_data *data){
    if(data && data->type == JS_STRING) return data->u.string;
    return NULL;
}

double js_as_number(js_data *data, double default_val){
    if(data && data->type == JS_NUMBER) return data->u.number;
    return default_val;
}

bool js_as_bool(js_data *data, bool default_val){
    if(data && data->type == JS_BOOL) return data->u.boolean;
    return default_val;
}

#endif //JS_READER_IMPLEMENTATION
#endif //JS_READER
