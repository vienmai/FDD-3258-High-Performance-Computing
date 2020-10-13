CXX := mpicc
CXXFLAGS := -Wall -O2

SRC := $(wildcard *.c)
EXE := $(patsubst %.c,%,$(filter %.c,$(SRC)))

.PHONY: all
all: $(EXE)

# pattern rules
% : %.c
	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY: clean
clean:
	$(RM) $(EXE)
