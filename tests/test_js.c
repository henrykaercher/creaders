#define JS_READER_IMPLEMENTATION
#include "../src/js_reader.h"
#include <stdio.h>
#include <stdlib.h>

void print_json(js_data *data, int indent);

int main(int argc, char **argv){
    if(argc < 2){
        printf("Usage: %s <path_to_file.json>\n", argv[0]);
        return 1;
    }

    const char *test_file = argv[1];
    printf("Reading the file: %s\n\n", test_file);

    js_data *root = NULL;
    js_result res = json_parse(test_file, &root);

    if(res == JS_OK){
        printf("--- JSON READ SUCCESSFULLY  ---\n");
        print_json(root, 0);
        printf("-----------------------------\n");
    }
	else if(res == JS_ERR_PARSE){
        printf("Erro: Invalid JSON format. Verify the syntax.\n");
    }
	else if(res == JS_ERR_MEMORY){
        printf("Erro: Can't find the file or out of memory\n");
    }

    if(root){
        js_free(root);
        printf("\nMemory free.\n");
    }

    return 0;
}

void print_json(js_data *data, int indent){
    if(!data) return;

    for(int i = 0; i < indent; i++) printf("  ");

    switch(data->type){
        case JS_NULL:
            printf("null\n");
            break;
        case JS_BOOL:
            printf("%s\n", data->u.boolean ? "true" : "false");
            break;
        case JS_NUMBER:
            printf("%f\n", data->u.number);
            break;
        case JS_STRING:
            printf("\"%s\"\n", data->u.string);
            break;
        case JS_ARRAY:
            printf("[\n");
            for(size_t i = 0; i < data->u.array.count; i++){
                print_json(&data->u.array.values[i], indent + 1);
            }
            for(int i = 0; i < indent; i++) printf("  ");
            printf("]\n");
            break;
        case JS_OBJECT:
            printf("{\n");
            for(size_t i = 0; i < data->u.object.count; i++){
                for(int j = 0; j < indent + 1; j++) printf("  ");
                printf("\"%s\": ", data->u.object.members[i].key);
                
                if(data->u.object.members[i].value.type == JS_OBJECT || 
                    data->u.object.members[i].value.type == JS_ARRAY){
                    printf("\n");
                    print_json(&data->u.object.members[i].value, indent + 1);
                }
				else{
                    print_json(&data->u.object.members[i].value, 0); 
                }
            }
            for(int i = 0; i < indent; i++) printf("  ");
            printf("}\n");
            break;
    }
}
