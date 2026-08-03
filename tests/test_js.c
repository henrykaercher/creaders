#define JS_READER_IMPLEMENTATION
#include "../src/js_reader.h"

#include <stdio.h>
#include <stdlib.h>

static void print_indent(int indent);
static void print_json(const js_data *data, int indent);
static void print_type(const js_data *data);

int main(int argc, char **argv){
    if(argc != 2){
        fprintf(stderr, "Usage: %s <file.json>\n", argv[0]);
        return EXIT_FAILURE;
    }

    js_data *root = NULL;

    printf("Opening: %s\n\n", argv[1]);

    js_result result = json_parse(argv[1], &root);

    if(result != JS_OK){
        switch(result){
            case JS_ERR_PARSE:
                fprintf(stderr, "Parse error.\n");
                break;

            case JS_ERR_MEMORY:
                fprintf(stderr, "Memory/File error.\n");
                break;

            default:
                fprintf(stderr, "Unknown error.\n");
        }

        return EXIT_FAILURE;
    }

    printf("========== Parsed JSON ==========\n\n");
    print_json(root, 0);

    printf("\n========== Root ==========\n");
    print_type(root);

    if(root->type == JS_OBJECT){
        printf("Members : %zu\n", root->u.object.count);
    }

    if(root->type == JS_ARRAY){
        printf("Elements: %zu\n", root->u.array.count);
    }

    printf("\n========== API Test ==========\n");

    js_data *item = js_get_object(root, "items");

    if(item){
        printf("Found key \"item\"\n");
        print_type(item);

        switch(item->type){
            case JS_STRING:
                printf("Value : %s\n", js_as_string(item));
                break;

            case JS_NUMBER:
                printf("Value : %.2f\n", js_as_number(item, 0));
                break;

            case JS_BOOL:
                printf("Value : %s\n",
                       js_as_bool(item, false) ? "true" : "false");
                break;

            default:
                printf("Complex value\n");
                break;
        }
    }else{
        printf("Key \"items\" not found.\n");
    }

    js_free(root);

    printf("\nMemory released successfully.\n");

    return EXIT_SUCCESS;
}

static void print_indent(int indent){
    while(indent--)
        printf("    ");
}

static void print_type(const js_data *data){
    static const char *names[] = {
        "OBJECT",
        "ARRAY",
        "STRING",
        "NUMBER",
        "BOOLEAN",
        "NULL"
    };

    printf("Type  : %s\n", names[data->type]);
}

static void print_json(const js_data *data, int indent){
    switch(data->type){

        case JS_NULL:
            printf("null");
            break;

        case JS_BOOL:
            printf("%s", data->u.boolean ? "true" : "false");
            break;

        case JS_NUMBER:
            printf("%g", data->u.number);
            break;

        case JS_STRING:
            printf("\"%s\"", data->u.string);
            break;

        case JS_ARRAY:

            printf("[\n");

            for(size_t i = 0; i < data->u.array.count; i++){

                print_indent(indent + 1);

                print_json(&data->u.array.values[i], indent + 1);

                if(i + 1 != data->u.array.count)
                    printf(",");

                printf("\n");
            }

            print_indent(indent);
            printf("]");
            break;

        case JS_OBJECT:

            printf("{\n");

            for(size_t i = 0; i < data->u.object.count; i++){

                print_indent(indent + 1);

                printf("\"%s\": ",
                       data->u.object.members[i].key);

                print_json(&data->u.object.members[i].value,
                           indent + 1);

                if(i + 1 != data->u.object.count)
                    printf(",");

                printf("\n");
            }

            print_indent(indent);
            printf("}");
            break;
    }
}
