CC := cc

CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Werror \
          -Wmissing-prototypes -Wstrict-prototypes

.PHONY: run clean

run:
ifndef TEST
	$(error Usage: make run TEST=<name>)
endif
	$(CC) $(CFLAGS) $(TEST).c -o $(TEST)
	./$(TEST)

clean:
	rm -f test_*
