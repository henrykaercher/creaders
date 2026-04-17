#ifndef MK_READER_H
#define MK_READER_H

#include <stddef.h>

typedef enum {
    MK_HEADING,
    MK_PARAGRAPH,
    MK_LIST,
    MK_CODE,
	MK_IMAGE,
    MK_LINK,
	MK_QUOTE,
    MK_META
} mk_block_type;

typedef struct{
    mk_block_type type;
    char *text;
    int level;
    int pos;
} mk_block;

typedef struct{
    mk_block *blocks;
    size_t block_count;
} mk_document;

char *mk_strndup(const char *src, size_t n);
char *load_content(const char *file_path);
mk_document read_content(const char *content);
void mk_document_free(mk_document *doc);

#ifdef MK_READER_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *mk_strndup(const char *src, size_t n){
    char *dst = malloc(n + 1);
    if (!dst) return NULL;

    memcpy(dst, src, n);
    dst[n] = '\0';
    return dst;
}

char *load_content(const char *file_path){
	FILE* mk_file = fopen(file_path, "rb");

	if(!mk_file){
        perror("fopen");
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

	fclose(mk_file);
	return content;
}

mk_document read_content(const char *content){
    mk_document doc = {0};
    const char *p = content;
    int position_order = 0;

    while(*p){
        const char *line_start = p;
        const char *line_end = strchr(p, '\n');

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
        if(!new_blocks) break;

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

                position_order++;
                current->pos = position_order;
            }else{
                current->type = MK_PARAGRAPH;
                current->text = mk_strndup(line_start, len);
                position_order++;
                current->pos = position_order;
            }
        }
        else if(len >= 2 && line_start[0] == '-' && line_start[1] == ' '){
            current->type = MK_LIST;
            current->text = mk_strndup(line_start + 2, len - 2);
            position_order++;
            current->pos = position_order;
        }
        else if(len >= 3 && strncmp(line_start, "```", 3) == 0){
            current->type = MK_CODE;

            const char *code_start = p;
            const char *code_end = NULL;

            while(*p){
                const char *next_line = p;
                const char *newline = strchr(p, '\n');

                size_t next_len;
                if(newline){
                    next_len = (size_t)(newline - next_line);
                    p = newline + 1;
                } else {
                    next_len = strlen(next_line);
                    p += next_len;
                }

                if(next_len >= 3 && strncmp(next_line, "```", 3) == 0){
                    code_end = next_line;
                    break;
                }
            }

            if(code_end){
                size_t code_len = code_end - code_start;
                current->text = mk_strndup(code_start, code_len);
            }else{
                size_t code_len = strlen(code_start);
                current->text = mk_strndup(code_start, code_len);
            }

            position_order++;
            current->pos = position_order;
        }
		else if((line_start[0] == '!' && line_start[1] == '[') || (line_start[0] == '[')){
			char *url_start = strchr(line_start, '(');
			char *url_end   = strchr(line_start, ')');

			if(url_start && url_end && url_end > url_start){
				current->type =	(line_start[0] == '!') ? MK_IMAGE : MK_LINK;
				current->text = mk_strndup(url_start + 1, (size_t)(url_end - url_start - 1));
                position_order++;
                current->pos = position_order;
			}
		}
		else if(line_start[0] == '>'){
            current->type = MK_QUOTE;

            const char *start = line_start + 1;
            if(*start == ' ') start++;

            size_t new_len = (line_start + len) - start;
            current->text = mk_strndup(start, new_len);

            position_order++;
            current->pos = position_order;
        }
        else if(line_start[0] == '@'){
            current->type = MK_META;

            const char *start = line_start + 1;
            if(*start == ' ') start++;

            size_t new_len = (line_start + len) - start;
            current->text = mk_strndup(start, new_len);

            position_order++;
            current->pos = position_order;
        }
        else{
            current->type = MK_PARAGRAPH;
            current->text = mk_strndup(line_start, len);
            position_order++;
            current->pos = position_order;
        }

        doc.block_count++;
    }

    return doc;
}

void mk_document_free(mk_document *doc){
    if(!doc || !doc->blocks) return;

    for(size_t i = 0; i < doc->block_count; i++){
        free(doc->blocks[i].text);
    }

    free(doc->blocks);
    doc->blocks = NULL;
    doc->block_count = 0;
}

#endif
#endif
