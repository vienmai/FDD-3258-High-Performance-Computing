NVCXX=nvcc
NVCXXFLAGS=-Icuda_helper -O3 -arch=sm_30
#CXXFLAGS=-Icuda_helper -O3 -arch=sm_37
# CXXFLAGS=-Icuda_helper --gpu-architecture=compute_50 --gpu-code=sm_50
# CXXFLAGS=-Icuda_helper --gpu-architecture=compute_50 --gpu-code=sm_37
# CXXFLAGS=-Icuda_helper --gpu-architecture=compute_37 --gpu-code=compute_37,sm_50
# CXXFLAGS=-Icuda_helper --gpu-architecture=compute_37 --gpu-code=sm_37

SRC := $(wildcard *.cu)
EXE := $(patsubst %.cu,%,$(filter %.cu,$(SRC)))

.PHONY: all
all: $(EXE)

# pattern rules
% : %.cu
	$(NVCXX) $(NVCXXFLAGS) -o $@ $<

.PHONY: clean
clean:
	$(RM) $(EXE)
