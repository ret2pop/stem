CC := gcc 
SRCDIR := src
BUILDDIR := build
TARGET := stem
SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -MMD -MP
LIB := -L lib -lm
INC := -I include

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET) $(LIB)"; $(CC) $^ -o $(TARGET) $(LIB) -O3

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT) Makefile
	@echo " Building..."
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $< -save-temps -O3

compile_flags.txt: Makefile
	printf "%s\n" $(CFLAGS) $(INC) >$@

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

install:
	cp $(TARGET) /usr/local/bin/
	mkdir -p /usr/local/share/stem/
	cp -r stemlib/ /usr/local/share/stem/

doc:
	doxygen

site:
	doxygen
	rsync -uvrP --delete-after "html/" root@nullring.xyz:/var/www/stemdoc

-include $(OBJECTS:.o=.d)
