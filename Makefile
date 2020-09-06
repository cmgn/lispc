CC := g++
CPPFLAGS := -Wall -Werror -O3
MODULES := expr.o reader.o

all: $(MODULES)
	$(CC) $(CPPFLAGS) -o main $(MODULES) main.cc

clean:
	$(RM) $(wildcard *.o) main