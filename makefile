
#go 64 bits????
CC=gcc
CFLAGS=-Wall -c
LDFLAGS=-lmingw32 -lSDL2main -lSDL2


SOURCEDIR=src
OBJDIR=obj
HEADERDIR=include
EXECUTABLE=chip8emu
SOURCES=$(wildcard src/*.c)
OBJECTS=$(subst src/, obj/, $(SOURCES:.c=.o))

all: $(SOURCEDIR) $(OBJDIR) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $^ -o $@  $(LDFLAGS)

obj/%.o: src/%.c
	$(CC) -I./$(HEADERDIR) $(CFLAGS) $< -o $@

$(SOURCEDIR):
	@if [ ! -d "$(SOURCEDIR)" ];then\
		mkdir -p $(SOURCEDIR);\
	fi
$(OBJDIR):
	@if [ ! -d "$(OBJDIR)" ];then\
		mkdir -p $(OBJDIR);\
	fi

.PHONY: clean
clean:
	@if [ -e $(EXECUTABLE) ];\
	then\
		rm -f $(EXECUTABLE);\
	fi;
	@if [ -e $(OBJDIR) ];\
	then \
		rm -fr $(OBJDIR);\
	fi;