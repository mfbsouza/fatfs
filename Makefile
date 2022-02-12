# Makefile by Matheus Souza (github.com/mfbsouza)

# project name
PROJECT  := fatfs

# paths
BUILDDIR := ./build
DBGDIR   := $(BUILDDIR)/debug
RELDIR   := $(BUILDDIR)/release
INCDIR   := ./include


# compiler settings
CC  := $(PREFIX)gcc
LD  := $(PREFIX)gcc
OD  := $(PREFIX)objdump

ifeq ($(DEBUG),1)
	BINDIR    := $(DBGDIR)
	OBJDIR    := $(DBGDIR)/obj
	ALLCFLAGS += -g -O0 -DDEBUG
else
	BINDIR    := $(RELDIR)
	OBJDIR    := $(RELDIR)/obj
	ALLCFLAGS += -g -O3
endif

# final compilation flags
CFLAGS   := -std=c99 -Wall $(ALLCFLAGS) -MMD -MP
LDFLAGS  := $(ALLLDFLAGS)

# sources to compile
ALLCSRCS   += $(shell find ./src ./include -type f -name *.c)

# sources settings
CSRCS    := $(ALLCSRCS)
SRCPATHS := $(sort $(dir $(CSRCS)))

# objects settings
COBJS   := $(addprefix $(OBJDIR)/, $(notdir $(CSRCS:.c=.o)))
OBJS    := $(COBJS)
DEPS    := $(OBJS:.o=.d)

# paths where to search for sources
VPATH   = $(SRCPATHS)

# output
OUTFILES := $(BINDIR)/$(PROJECT) $(BUILDDIR)/$(PROJECT).lst

# targets
.PHONY: all clean

all: $(OBJDIR) $(BINDIR) $(OBJS) $(OUTFILES)

# targets for the dirs
$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(BINDIR):
	@mkdir -p $(BINDIR)

# target for c objects
$(COBJS) : $(OBJDIR)/%.o : %.c
ifeq ($(VERBOSE),1)
	$(CC) -c $(CFLAGS) -I $(INCDIR) $< -o $@
else
	@echo -e "[CC]\t$<"
	@$(CC) -c $(CFLAGS) -I $(INCDIR) $< -o $@
endif

# target for ELF file
$(BINDIR)/$(PROJECT): $(OBJS)
ifeq ($(VERBOSE),1)
	$(LD) $(LDFLAGS) $(OBJS) -o $@
else
	@echo -e "[LD]\t./$@"
	@$(LD) $(LDFLAGS) $(OBJS) -o $@
endif

# target for disassembly and sections header info
$(BUILDDIR)/$(PROJECT).lst: $(BINDIR)/$(PROJECT)
ifeq ($(VERBOSE),1)
	$(OD) -h -S $< > $@
else
	@echo -e "[OD]\t./$@"
	@$(OD) -h -S $< > $@
endif

# target for cleaning files
clean:
	rm -rf $(BUILDDIR)

# Include the dependency files, should be the last of the makefile
-include $(DEPS)
