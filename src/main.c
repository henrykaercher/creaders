#include <stdio.h>
#include "mk_reader.h"

int main(){
	char *print = load_content("/home/muser/devtools/c/creaders/tests/markdown_test.md");
	mk_document doc = read_content(print);
	printf("%s\n", doc.blocks->text);
	free(print);

	return 0;
}

