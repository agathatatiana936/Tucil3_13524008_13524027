CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS  :=

HAVE_RAYLIB := $(shell pkg-config --exists raylib && echo 1 || echo 0)

ifeq ($(HAVE_RAYLIB),1)
    CXXFLAGS += -DHAVE_RAYLIB $(shell pkg-config --cflags raylib)
    LDFLAGS  += $(shell pkg-config --libs raylib)
endif

ifeq ($(OS),Windows_NT)
    LDFLAGS += -lole32 -lshell32 -lcomdlg32
else
    CXXFLAGS += -pthread
    LDFLAGS  += -pthread
endif

SRCDIR   := src
INCDIR   := include
BUILDDIR := build
TARGET   := $(BUILDDIR)/ice_solver

SRCS := $(shell find $(SRCDIR) -name '*.cpp')
OBJS := $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))

.PHONY: build run clean deps

build: $(TARGET)

run: build
	./$(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -rf $(BUILDDIR)/*

deps: $(SRCS)
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -MM $^ | sed 's|^\([a-zA-Z0-9_]*\.o:\)|$(BUILDDIR)/\1|' > $(BUILDDIR)/deps.mk

-include $(BUILDDIR)/deps.mk
