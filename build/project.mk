# project.mk

# project-name
PROJECT = tab

# all -I and -L options
DIRECTORY_OPTIONS =

# extra compiler flags
COMPILER_OPTIONS = -Wall -std=gnu++17

# all -l options
LIBRARIES = -lSDL2 -lSDL_ttf -lGL

# cpps and headers relative to project/src 
CPP_FILES = \
font.cpp \
sdl_font.cpp \
opengl_font.cpp \
scrollbar.cpp \
cell.cpp \
table.cpp

HEADER_FILES = \
font.h \
sdl_font.h \
opengl_font.h \
scrollbar.h \
cell.h \
table.h
