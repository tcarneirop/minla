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

CPP_FLAGS = -std=c++11 -O3 -march=native -fopenmp -ffast-math
CXX = g++

csource: cuda
	@echo 
	@echo " ### Building the Chapel code... ### "
	@echo 

	$(CXX) $(CPP_FLAGS) $(C_SOURCES) main.cpp -o $(BUILD_DIR)/minla
	
	@echo 
	@echo " ### Compilation done ### "
	
cuda: 
	@echo 
	@echo " ### starting CUDA compilation ### "
	@echo 

.PHONY: clean
clean:
	$(RM) $(LIBRARY_DIR)/*.so
	$(RM) $(BUILD_DIR)/minla


