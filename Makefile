TEST_SOURCES=$(wildcard *_test.c)
TEST_OBJECTS=$(patsubst %.c, %.o, $(SOURCES))
TEST_TARGETS=$(patsubst %.c, %, $(SOURCES))

all: tests

cordless: $(TEST_TARGETS)
	cp $^ /home/robot/cordless/

clean:
	rm -f $(TEST_OBJECTS) $(TEST_TARGETS)

tests: $(TEST_TARGETS)

%.o: %.c
	gcc $< -c -o $@ -I/usr/local/include

%: %.o
	gcc $< -o $@ -L/usr/local/lib -lzlog -lpthread -lev3dev-c

.SUFFIXES:

.PHONY: all cordless tests clean
