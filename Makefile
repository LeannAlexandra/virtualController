# Compiler and flags
CC := gcc
CXX := g++
CFLAGS := -Wall -I./include
CXXFLAGS := -Wall -I./include
LDFLAGS := -L./lib -ludev  # Link against libudev
LDLIBS := -lm

# Directories
SRCDIR := src
OBJDIR := obj
LIBDIR := lib

# Source files
CSOURCES := $(wildcard $(SRCDIR)/*.c)
CXXSOURCES := $(wildcard $(SRCDIR)/*.cpp)
COBJECTS := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(CSOURCES))
CXXOBJECTS := $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(CXXSOURCES))

# Executable names
EXECUTABLE := virtualController
UDEV_EXECUTABLE := udevInfo  # Name of the new program using libudev

# Targets
all: $(EXECUTABLE) $(UDEV_EXECUTABLE)

$(EXECUTABLE): $(COBJECTS) $(CXXOBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(UDEV_EXECUTABLE): udev_info.c  # Assuming you have a separate .c file for the libudev program
	$(CC) $(CFLAGS) -o $@ $^ -ludev

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJDIR) $(EXECUTABLE) $(UDEV_EXECUTABLE)

.PHONY: all clean
