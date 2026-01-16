#ifndef MK_READER_H
#define MK_READER_H

#include <stdio.h>
#include <stdlib.h>

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

/*
mk_document {
    blocks[0] = HEADING("# Title")
    blocks[1] = PARAGRAPH("Some paragraph text.")
    blocks[2] = HEADING("## Subtitle")
    blocks[3] = LIST("- item 1\n- item 2")
    blocks[4] = CODE("int x = 0;")
}*/

static inline void load_content(char *file_path){
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

	printf("Size of file: %zu\nNumber of lines: %d\n", bytes_read, lines);

	fclose(mk_file);
	free(content);
}

/*static inline void unload_content(mk_document *document){
	if(!document) return;

	for(size_t i = 0; i < document->count; i++){
        free(document->blocks[i].text);
    }

    free(document->blocks);
    document->blocks = NULL;
    document->count = 0;
}*/

#endif
