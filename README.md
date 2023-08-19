# Parallel Object Matching with CUDA and OpenMP

This project demonstrates parallel object matching in images using CUDA for GPU acceleration and OpenMP for multi-threading. The goal is to efficiently match objects within images using parallel processing techniques.

## Table of Contents

- [Introduction](#introduction)
- [Project Structure](#project-structure)
- [Prerequisites](#prerequisites)
- [Building and Running](#building-and-running)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## Introduction

This project showcases the parallel processing capabilities of CUDA and OpenMP in matching objects within images. The codebase includes a C source file for MPI-based distributed processing and a CUDA source file for GPU acceleration of object matching.

## Project Structure

The project consists of the following components:

- `main.c`: The main source file containing the MPI-based distributed processing logic.
- `fileReader.c`: Source file containing functions to read input data.
- `cudaConv.cu`: CUDA source file implementing GPU-accelerated object matching.
- `header.h`: Header file containing function prototypes and structures.
- `Makefile`: Makefile to compile and build the project.
- `inc/`: Directory containing header files.
- `data/`: Directory containing input data files.

## Prerequisites

Before building and running the project, ensure you have the following prerequisites installed:

- GCC (GNU Compiler Collection) for C compilation.
- NVIDIA CUDA Toolkit for GPU acceleration.
- OpenMP support in GCC for multi-threading.
- MPI libraries (assumed based on the provided code).

## Building and Running

1. Clone this repository to your local machine.

2. Navigate to the project directory in your terminal.

3. Edit the `Makefile` if necessary to match your system configuration.

4. Run the following commands:

   ```bash
   make build   # Compile and build the project
   make run     # Run the compiled executable
