SHELL := /bin/bash

BUILD_DIR := ./bin
CUDA_SRC_DIR := ./kernels
C_SRC_DIR := ./src
CUDA_PATH := /usr/local/cuda
CULIB := $(CUDA_PATH)lib64

CUDA_INCLUDE_DIR := $(CUDA_PATH)/include
CUDA_LIB_DIR := $(CUDA_PATH)/lib
LIBRARY_DIR := ./libs
C_SOURCES := $(shell find $(C_SRC_DIR) -name '*.cpp')

CPP_FLAGS = -std=c++14 -O3 -march=native -fopenmp -ffast-math
NVCC_FLAGS = -fPIC -O3
CXX = g++

csource: cuda
	@echo 
	@echo " ### Building cpp code... ### "
	@echo 

	$(CXX) -L$(LIBRARY_DIR) -lminla  $(CPP_FLAGS) $(C_SOURCES) main.cpp -o $(BUILD_DIR)/minla

	@echo 
	@echo " ### Compilation done ### "
	
cuda: 
	@echo 
	@echo " ### starting CUDA compilation ### "
	@echo 
	$(CUDA_PATH)/bin/nvcc --shared -o $(LIBRARY_DIR)/libminla.so $(CUDA_SRC_DIR)/minla_kernels.cu --compiler-options $(NVCC_FLAGS) -I$(CUDA_INCLUDE_DIR) -L$(CUDA_LIB_DIR) -lcudart
	
	#$(CUDA_PATH)/bin/nvcc --shared -o $(LIBRARY_DIR)/libminla.so $(CUDA_SRC_DIR)/GPU_aux.cu  --compiler-options '-fPIC -O3' -I$(CUDA_INCLUDE_DIR) -L$(CUDA_LIB_DIR) -lcudart
	


.PHONY: clean
clean:
	$(RM) $(LIBRARY_DIR)/*.so
	$(RM) $(BUILD_DIR)/minla


