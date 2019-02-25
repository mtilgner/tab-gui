# include DIY-path
include build/paths.mk

# include project-variables
include build/project.mk

# some variables
CC = g++

OUTPUT = libDIY_$(PROJECT).so

SRCDIR = src
DEPDIR = dep
OBJDIR = obj

INCDIR = $(DIY)/include/$(PROJECT)
LIBDIR = $(DIY)/lib

# sources is again a list (file in files : src/file)
SOURCES = $(addprefix $(SRCDIR)/,$(CPP_FILES))
HEADERS = $(addprefix $(SRCDIR)/,$(HEADER_FILES))
# everything in files, suffix replaced, prefix (folder) added
OBJS = $(addprefix $(OBJDIR)/,$(CPP_FILES:.cpp=.o))
DEPENDENCIES = $(addprefix $(DEPDIR)/,$(CPP_FILES:.cpp=.d))

REBUILDABLES = $(OUTPUT) $(LIBDIR)/$(OUTPUT)

# make OUTPUT, copy headers, copy OUTPUT to LIBDIR
all : $(OUTPUT) includes
	cp $(OUTPUT) $(LIBDIR)

# delete rebuildables and everything in include-folder
# delete all files in obj and dep
clean :
	rm -f $(REBUILDABLES) $(INCDIR)/*
	find $(OBJDIR) -type f -delete
	find $(DEPDIR) -type f -delete
	
rebuild : clean all
	
# copy all headers to include-folder
includes : $(HEADERS)
	cp $(HEADERS) $(INCDIR)

# compile
# add SRCDIR to directories
$(OUTPUT) : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_OPTIONS) $(DIRECTORY_OPTIONS) -I$(SRCDIR) $(LIBRARIES) -shared -fPIC -o $(OUTPUT)

# rule to make any .o file
# $@ = target, $< = first prerequisite
# also make dependency file
# because if the .d file is a target it gets built when included!
$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CC) -c $(DIRECTORY_OPTIONS) -I$(SRCDIR) -fPIC -o $@ $<
	$(CC) -MM $(DIRECTORY_OPTIONS) -I$(SRCDIR) $< > $(@:$(OBJDIR)/%.o=$(DEPDIR)/%.d)
	build/fixdep.sh $(@:$(OBJDIR)/%.o=$(DEPDIR)/%.d) $@

# rule to make any .d file
# gen dependency file, then add obj/path/ to the .o string
# $(DEPDIR)/%.d : $(SRCDIR)/%.cpp
#	$(CC) -MM $< > $@
#	build/fixdep.sh $@ $(DEPDIR) $(OBJDIR)

# include all dependency files (if they exist)
-include $(DEPENDENCIES)


