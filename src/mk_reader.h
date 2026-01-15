#ifndef MK_READER_H
#define MK_READER_H

#include <stdio.h>

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

static inline mk_document load_content(char *file_path){
	mk_document doc = {0};
	char data[500];
	FILE* mk_file = fopen(file_path, "r");

	if(!mk_file){
		printf("No file detected\n");
		return doc;
	}

	while(fgets(data, 500, mk_file) != NULL){
		printf("%s", data);
	}

	fclose(mk_file);
	return doc;
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
