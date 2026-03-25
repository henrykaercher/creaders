#include <stdio.h>
#include "../src/mk_reader.h"

int main(void){
	//hardcoded path, change that for your own path
	char *buffer = load_content("/home/muser/devtools/c/creaders/tests/etc/markdown_test.md");
	mk_document doc = read_content(buffer);

	printf("%zu\n", doc.block_count);

	for(size_t i = 0; i < doc.block_count; i++){
		mk_block *b = &doc.blocks[i];

		printf("---- block %zu ----\n", i);
		printf("type : %d\n", b->type);
		printf("level: %d\n", b->level);
		printf("text : \"%s\"\n", b->text);
		printf("position : \"%d\"\n", b->pos);
	}
	mk_document_free(&doc);
	free(buffer);

	return 0;
}
