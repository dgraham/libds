TARGET  = libds.a
SOURCES = $(wildcard src/*.c)
OBJECTS = $(SOURCES:.c=.o)
TESTS   = $(wildcard test/*.c)
TESTX   = $(patsubst test/%.c,test-%,$(TESTS))
CFLAGS  = -O3 -Werror -Weverything -Wall -I src/

$(TARGET): test
	ar rcs $(TARGET) $(OBJECTS)

%.o: %.c
	cc -c $(CFLAGS) $< -o $@

test: $(TESTX)
	for x in $(TESTX); do ./$$x; done

test-%: test/%.c $(OBJECTS)
	cc $(CFLAGS) $< $(OBJECTS) -o $@

clean:
	rm -f $(OBJECTS)
	rm -f $(TESTX)
	rm -f $(TARGET)
