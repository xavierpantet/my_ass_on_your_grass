SRCDIR   := src
BINDIR   := bin
INCLUDES := -Iinclude/
OBJDIR   := .build
TESTDIR  := $(BINDIR)/tests

CC       := g++
#gcc -xc++ -lstdc++ -shared-libgcc
CFLAGS   := -Wall -Wextra -g -m32 -fno-stack-protector -z execstack -pthread -std=gnu++11 $(INCLUDES)
LFLAGS   := -lm

SRC      :=                                   	  \
	$(wildcard src/services/authorization/*.cpp)  \
	$(wildcard src/services/authentication/*.cpp) \
	$(wildcard src/services/commands/*.cpp)       \
	$(wildcard src/configuration/*.cpp)           \
	$(wildcard src/commands/*.cpp)				  \
	$(wildcard src/network/*.cpp)                 \
	$(wildcard src/helpers/*.cpp)                 \
	$(wildcard src/files/*.cpp)                   \
	$(wildcard src/user/*.cpp)				      \
	$(wildcard src/grass.cpp)                     \
	$(wildcard src/Exception.cpp)                 \
	$(wildcard tests/*.cpp)

OBJECTS := $(SRC:%.cpp=$(OBJDIR)/%.o)

all: build $(BINDIR)/server $(BINDIR)/client $(TESTDIR)

tests: refresh $(TESTDIR)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

$(BINDIR)/server: $(OBJECTS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(LFLAGS) $(INCLUDES) -o $(BINDIR)/server $(SRCDIR)/Server.cpp $(OBJECTS)

$(BINDIR)/client: $(OBJECTS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(LFLAGS) $(INCLUDES) -o $(BINDIR)/client $(SRCDIR)/ClientLauncher.cpp $(OBJECTS)

$(TESTDIR): $(OBJECTS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(LFLAGS) $(INCLUDES) -o $(BINDIR)/tests $(SRCDIR)/tests.cpp $(OBJECTS)


.PHONY: all build clean debug release refresh tests

build:
	@mkdir -p $(BINDIR)
	@mkdir -p $(OBJDIR)

debug: CFLAGS += -DDEBUG -g
debug: all

release: CFLAGS += -O2
release: all

clean:
	-@rm -rvf $(OBJDIR)/*
	-@rm -rvf $(BINDIR)/*

refresh: clean all
