CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS  :=

SRCDIR   := src
INCDIR   := include
BUILDDIR := build
TARGET   := $(BUILDDIR)/solver

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
