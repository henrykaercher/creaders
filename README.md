# CReaders

CReaders is a small collection of header-only C libraries for parsing structured text formats and exposing their content as block-based data structures.

## Extensions available

* Markdown
* JSON **(TODO)**
* XML **(TODO)**

> **Note:** These libraries do not convert between formats. Each parser is independent.

## How to use

Include the header for the format you want:

```c
#define MK_READER_IMPLEMENTATION
#include "mk_reader.h"
```

### Loading and parsing a file

```c
char *buffer = load_content("mkfile.md");
if (!buffer) {
    return 1;
}

mk_document doc = read_content(buffer);
```

The parser splits the file into blocks stored in `mk_document`.

### Accessing parsed data

```c
for (size_t i = 0; i < doc.block_count; i++) {
    mk_block *b = &doc.blocks[i];

    printf("type: %d\n", b->type);
    printf("text: \"%s\"\n", b->text);
    printf("level: %d\n", b->level);
    printf("position: %d\n\n", b->pos);
}
```

### Memory management

When finished, free all allocated memory:

```c
mk_document_free(&doc);
free(buffer);
```

## Notes

* `pos` represents the order of blocks in the document
* `level` is only meaningful for headings
* The parser is line-based and does not fully implement the Markdown specification

## Status

This project is experimental and under active development.

Expect rough edges and incomplete features.
