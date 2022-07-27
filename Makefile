CFLAGS= -g -Wall -std=c99 -I.
LDLIBS= 
TESTFLAGS= $(CFLAGS) -Isrc/
ifdef debug
	CFLAGS+=-DDEBUG
endif

all: check
bin:; mkdir -p bin/

bin/test_hashmap: src/unja_hashmap.c tests/test_hashmap.c | bin
	$(CC) $(TESTFLAGS) $^ -o $@

bin/test_template: src/unja_template.c src/unja_hashmap.c src/unja_vector.c tests/test_template.c vendor/mpc.c | bin 
	$(CC) $(TESTFLAGS) $^ -o $@

.PHONY: check
check: bin/test_hashmap bin/test_template
	for test in $^; do $$test || exit 1; done	

.PHONY: clean 
clean:; rm -r bin/
