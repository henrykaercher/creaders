# CReaders

**CReaders** is a collection of single-header C libraries for reading and parsing structured text formats.

Each parser is independent and exposes a simple C API designed for lightweight applications, command-line tools, game engines, configuration systems, and educational projects.

## Features

* Header-only libraries
* Written in ISO C
* No external dependencies
* Simple data structures
* Easy integration into existing projects

## Available libraries

| Library  | Status       |
| -------- | ------------ |
| Markdown | Stable       |
| JSON     | Experimental |
| XML      | Planned      |

Each library can be used independently.

## Installation

Clone the repository and run the installation script:

```sh
git clone https://github.com/henrykaercher/CReaders.git
cd CReaders
sudo ./install.sh
```

After installation the headers become available globally:

```c
#include <creaders/js_reader.h>
#include <creaders/mk_reader.h>
```

You can also copy the desired header directly into your project if you prefer not to install it.

## Usage

Every parser follows the same implementation pattern.

```c
#define JS_READER_IMPLEMENTATION
#include <creaders/js_reader.h>
```

Only one source file in your project should define the implementation macro.

Other source files only need:

```c
#include <creaders/js_reader.h>
```

## Example

```c
js_data *root = NULL;

if(json_parse("config.json", &root) == JS_OK){
    /* use the parsed data */
}

js_free(root);
```

## Project goals

CReaders focuses on simplicity rather than implementing every corner case of each specification.

The libraries are intended to be:

* Small
* Easy to understand
* Easy to embed
* Suitable for learning
* Suitable for lightweight applications

## Roadmap

* Improve JSON specification compliance
* XML parser
* Better diagnostics and error reporting
* Additional helper functions
* More examples and documentation
* Unit tests

## License

MIT License.

