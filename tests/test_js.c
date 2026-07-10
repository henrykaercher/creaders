#define JS_READER_IMPLEMENTATION
#include "../src/js_reader.h"

int main(void){
	char *file = js_load_content("etc/json_test.json");
	printf("%s\n", file);
	return 0;
}
