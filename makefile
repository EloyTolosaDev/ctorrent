CC = gcc
CFLAGS = -Wall -Wno-maybe-uninitialized # ...

SRCDIR = ./src
BUILDDIR = ./build

TARGET = ctorrent

# := evaluates the expression at the moment of the assignation
CFILES := $(shell find -type f -regex ".+\.[c]" | awk '{print $$1}')

__OBJ := $(shell find -type f -regex ".+\.[c]" | awk 'function basename(file) { sub(".*/", "", file); return file }  {print basename($$1)}')
OUTPUTS := $(__OBJ:.c=.o)
OUTPUTS := $(foreach output, $(OUTPUTS), $(BUILDDIR)/$(output))
OUTPUTS := $(filter-out $(BUILDDIR)/$(TARGET).o, $(OUTPUTS))

RULES := $(CFILES:.c=.o)
RULES := $(filter-out $(SRCDIR)/$(TARGET).o, $(RULES))

ctorrent: $(RULES)
	@echo "Building project file..."
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(TARGET).c -o $(BUILDDIR)/$(TARGET).o -I$(SRCDIR)
	$(CC) $(CFLAGS) $(OUTPUTS) $(BUILDDIR)/$(TARGET).o -o $(TARGET)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $(BUILDDIR)/$(shell basename $@) -I$(SRCDIR)

.PHONY: all
all: clean ctorrent

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)/* $(TARGET)

.PHONY: print
print:
	@echo objects: $(OUTPUTS)