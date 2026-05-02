CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS  :=

SRCDIR   := src
INCDIR   := include
BUILDDIR := build
TARGET   := $(BUILDDIR)/solver

SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR):
	@mkdir -p $(BUILDDIR)

clean:
	@rm -rf $(BUILDDIR)


deps: $(SRCS)
	$(CXX) $(CXXFLAGS) -MM $^ | sed 's|^\([a-zA-Z0-9_]*\.o:\)|$(BUILDDIR)/\1|' > $(BUILDDIR)/deps.mk

-include $(BUILDDIR)/deps.mk
