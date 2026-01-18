#ifndef MK_READER_H
#define MK_READER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static const char *heading_prefixes[] = {
    "# ", "## ", "### "
};

typedef enum {
    MK_HEADING,
    MK_PARAGRAPH,
    MK_LIST,
    MK_CODE,
    MK_IMAGE,
    MK_LINK
} mk_block_type;

typedef struct {
    mk_block_type type;
    char *text;
    int level;
} mk_block;

typedef struct {
    mk_block *blocks;
    size_t block_count;
} mk_document;

static inline char *void load_content(char *file_path){
	FILE* mk_file = fopen(file_path, "rb");

	if(!mk_file){
		printf("No file detected\n");
		return;
	}

	fseek(mk_file, 0, SEEK_END);
    long file_size = ftell(mk_file);
	rewind(mk_file);

	char *content = malloc(file_size + 1);
	if(!content){
		fclose(mk_file);
		return;
	}

	size_t bytes_read = fread(content, 1, file_size, mk_file);
	content[bytes_read] = '\0';

	int lines = 0;
	for(size_t i = 0; i < bytes_read; i++){
		if(content[i] == '\n') lines++;
	}

	if (bytes_read > 0 && content[bytes_read - 1] != '\n') {
        lines++;
    }

	//these prints are just for visual feedback, remove them latter
	printf("Size of file: %zu\nNumber of lines: %d\n", bytes_read, lines);
	printf(content);

	fclose(mk_file);
	return content;
}

static inline mk_document read_content(char *content){
    mk_document doc = {0};

	//this will show only one line, probably the last one, need to append the content
    char *line = strtok(content, "\n");
    while(line){
		//explain this
        mk_block *new_blocks = realloc(doc.blocks, sizeof(mk_block) * (doc.block_count + 1));
        if(!new_blocks){
            break;
        }
        doc.blocks = new_blocks;

        mk_block *current = &doc.blocks[doc.block_count];
        memset(current, 0, sizeof(mk_block));

        if(line[0] == '#'){
			//loop for diferent ammounts of '#'
            current->type = MK_HEADING;
            int level = 0;
            while(line[level] == '#') level++;
            current->level = level;
            current->text = strdup(line + level + 1);
        }
        else if((line[0] == '-' || line[0] == '*') && line[1] == ' '){
            current->type = MK_LIST;
            current->text = strdup(line + 2);
        }
        else{
            current->type = MK_PARAGRAPH;
            current->text = strdup(line);
        }

        doc.block_count++;
        line = strtok(NULL, "\n");
    }

    return doc;
}

#endif
