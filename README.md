# CReaders

CReaders is a small collection of a header-only C libraries for parsing structured text formats and exposing their content as block-based data structures.

## Extensions available
 - Markdown
 - JSON **(TODO)**
 - XML **(TODO)**

 > **Note:** None of these libs convert for one extension to another.

## How to use

Choose the header for the especific extension you wanna use and simple include in your project:

```c
#include "mk_reader.h"
```

To get the data from the file you will use:

```c
char *buffer = load_content("mkfile.md");
mk_document doc = read_content(buffer);
```

Now all the content of the file is inside 'doc' separated by blocks, they can be accessed with:

```c
doc.block_count;
doc.blocks[i];
doc.blocks[i]->text;
doc.blocks[i]->type;
doc.blocks[i]->level;
```

For example you can use:

```c
for(size_t i = 0; i < doc.block_count; i++){
    mk_block *b = &doc.blocks[i];
    printf("text : \"%s\"\n", b->text);
}
```

And when finish to use remember to free the memory with:

```c
mk_document_free(&doc);
free(buffer);
```
