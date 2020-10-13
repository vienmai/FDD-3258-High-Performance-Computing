CXX :=mpicc
CXXFLAGS :=-Wall -g
LDFLAGS :=-lm 

SRC := $(filter-out pi.c, $(wildcard *.c))
EXE := $(patsubst %.c, %, $(filter %.c,$(SRC)))
INCS := pi.c

.PHONY: all
all: $(EXE)

# pattern rules
% : %.c 
	$(CXX) $(CXXFLAGS) -o $@ $< $(INCS) $(LDFLAGS)

.PHONY: clean
clean:
	$(RM) $(EXE)
