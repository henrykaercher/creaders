#ifndef MK_READER_H
#define MK_READER_H

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
	//load using malloc
}

static inline void unload_content(char *file){
	//unload using free
	free(file);
}

#endif
