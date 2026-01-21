#ifndef MK_READER_H
#define MK_READER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static inline char *mk_strndup(const char *src, size_t n){
    char *dst = malloc(n + 1);
    if (!dst)
        return NULL;

    memcpy(dst, src, n);
    dst[n] = '\0';
    return dst;
}

static inline char *load_content(char *file_path){
	FILE* mk_file = fopen(file_path, "rb");

	if(!mk_file){
		printf("No file detected\n");
		return NULL;
	}

	fseek(mk_file, 0, SEEK_END);
    long file_size = ftell(mk_file);
	rewind(mk_file);

	char *content = malloc(file_size + 1);
	if(!content){
		fclose(mk_file);
		return NULL;
	}

	size_t bytes_read = fread(content, 1, file_size, mk_file);
	content[bytes_read] = '\0';

	int lines = 0;
	for(size_t i = 0; i < bytes_read; i++){
		if(content[i] == '\n') lines++;
	}

	if(bytes_read > 0 && content[bytes_read - 1] != '\n'){
        lines++;
    }

	fclose(mk_file);
	return content;
}

static inline mk_document read_content(char *content){
    mk_document doc = {0};
    char *p = content;

    while(*p){
        char *line_start = p;
        char *line_end = strchr(p, '\n');

        size_t len;
        if(line_end){
            len = (size_t)(line_end - line_start);
            p = line_end + 1;
        }else{
            len = strlen(line_start);
            p += len;
        }

        if(len == 0) continue;

        mk_block *new_blocks = realloc(doc.blocks, sizeof(mk_block) * (doc.block_count + 1));
        if(!new_blocks)
            break;

        doc.blocks = new_blocks;
        mk_block *current = &doc.blocks[doc.block_count];
        memset(current, 0, sizeof(mk_block));

        if(line_start[0] == '#'){
            int level = 0;
            while(level < (int)len && line_start[level] == '#') level++;

            if(level < (int)len && line_start[level] == ' '){
                current->type = MK_HEADING;
                current->level = level;
                current->text = mk_strndup(line_start + level + 1, len - level - 1);
            }else{
                current->type = MK_PARAGRAPH;
                current->text = mk_strndup(line_start, len);
            }
        }
        else if(len >= 2 && line_start[0] == '-' && line_start[1] == ' '){
            current->type = MK_LIST;
            current->text = mk_strndup(line_start + 2, len - 2);
        }
        else if(len >= 3 && line_start[0] == '`' && line_start[1] == '`' && line_start[2] == '`'){
            current->type = MK_CODE;
			char *code_start = p; 
			char *code_end = strstr(code_start, "```\n");
			size_t code_len;
			if(code_end){
				code_len = (size_t)(code_end - code_start);
				p = code_end + 4;
			} else {
				code_len = strlen(code_start);
				p += code_len;
			}
            current->text = mk_strndup(code_start, code_len);
		}
		else if((line_start[0] == '!' && line_start[1] == '[') || (line_start[0] == '[')){
			char *url_start = strchr(line_start, '(');
			char *url_end   = strchr(line_start, ')');

			if(url_start && url_end && url_end > url_start){
				current->type =	(line_start[0] == '!') ? MK_IMAGE : MK_LINK;
				current->text = mk_strndup(url_start + 1, (size_t)(url_end - url_start - 1));
			}
		}
        else{
            current->type = MK_PARAGRAPH;
            current->text = mk_strndup(line_start, len);
        }

        doc.block_count++;
    }

    return doc;
}

static inline void mk_document_free(mk_document *doc){
    if(!doc || !doc->blocks)
        return;

    for(size_t i = 0; i < doc->block_count; i++){
        free(doc->blocks[i].text);
    }

    free(doc->blocks);
    doc->blocks = NULL;
    doc->block_count = 0;
}

#endif
