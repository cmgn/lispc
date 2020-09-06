CC := g++
CPPFLAGS := -Wall -Werror -O3
MODULES := expr.o reader.o

all: $(MODULES)
	$(CC) $(CPPFLAGS) -o main $(MODULES) main.cc

check: all
	./test.sh

clean:
	$(RM) $(wildcard *.o) main