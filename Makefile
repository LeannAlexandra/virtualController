# Compiler and flags
CC := gcc
CXX := g++
CFLAGS := -Wall -I./include
CXXFLAGS := -Wall -I./include
LDFLAGS := -L./lib -ludev
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
UDEV_EXECUTABLE := udevInfo

# Targets
all: $(EXECUTABLE) 
# $(UDEV_EXECUTABLE)

$(EXECUTABLE): $(COBJECTS) $(CXXOBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(UDEV_EXECUTABLE): $(OBJDIR)/udev_info.o
	$(CC) $(CFLAGS) -o $@ $^ -ludev

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJDIR) $(EXECUTABLE) 
# $(UDEV_EXECUTABLE)

.PHONY: all clean
