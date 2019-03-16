IDIR := Include
SDIR := Source
ODIR := .OBJ

CC := gcc
CFLAGS := -Wall -Ofast -I$(IDIR) -MMD

_SRC := $(shell find $(SDIR) -name "*.c")
OBJ := $(patsubst $(SDIR)/%.c, $(ODIR)/%.o, $(_SRC))
DEPS := $(patsubst $(SDIR)/%.c, $(ODIR)/%.d, $(_SRC))

# HEADERS = $(shell find $(IDIR) -name "*.h")

$(ODIR):
	@mkdir $(ODIR)

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

bts: $(ODIR) $(OBJ)
	$(CC) $(OBJ) -o $@ 

.PHONY: clean

clean:
	rm -rf bts $(ODIR)

$(VERBOSE).SILENT:

-include $(DEPS)