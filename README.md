# Row-wise CLCSt: Implementation of Algorithms for the Constrained Longest Common Subsequence Problem with t-length Substrings

This repository contains the code implementation for the **Constrained Longest Common Subsequence (CLCSt)** problem with t-length substrings, as presented by **Zhu, K., et al. (2021)** in the paper "**Algorithms for the Constrained Longest Common Subsequence Problem with t-length Substrings**". This research was presented at the *National Computer Symposium 2021 (NCS 2021)* in Taichung, Taiwan.

## Problem Overview

The **Constrained Longest Common Subsequence (CLCSt)** problem is a variant of the classical Longest Common Subsequence (LCS) problem, where the subsequences must consist of substrings of fixed length `t`. Given two sequences, the goal is to find the longest subsequence common to both sequences that satisfies the constraint of containing only substrings of length `t`. This problem arises in various fields, including bioinformatics, data mining, and text processing, where substring matching is of critical importance.

### Problem Formulation:

Let `X = x1, x2, ..., xn` and `Y = y1, y2, ..., ym` be two sequences. The CLCSt problem seeks to find the longest subsequence `L` such that:

1. `L` is a subsequence of both `X` and `Y`.
2. Each element in `L` is a substring of length `t`.

### Key Contributions:

The paper presents novel algorithms that efficiently solve the CLCSt problem by leveraging dynamic programming and row-wise optimization techniques. The authors focus on reducing the computational complexity of the problem, which is especially challenging for large sequences and when the constraint `t` is significant.

## Paper Reference

- **Title**: Algorithms for the Constrained Longest Common Subsequence Problem with t-length Substrings  
- **Authors**: Zhu, K., et al.  
- **Conference**: National Computer Symposium 2021 (NCS 2021), Taichung, Taiwan  
- **Link to Paper**: (If available, you may add the URL here)

## Features

- **Efficient Algorithms**: Implements dynamic programming-based algorithms for solving the CLCSt problem.
- **Row-wise Optimization**: Utilizes row-wise dynamic programming to optimize the matching of substrings and subsequences.
- **Flexible Input Handling**: The code supports multiple input formats and allows users to set the length `t` of substrings.
- **Test Cases**: Includes example datasets and test cases to validate the implementation.
- **Performance**: Designed for large-scale sequence analysis by reducing the time complexity compared to brute-force methods.

## Getting Started

### Clone the Repository

Start by cloning the repository to your local machine:

```bash
git clone https://github.com/zkxshg/Row-wise-CLCSt.git
cd Row-wise-CLCSt
```

### Prerequisites

The code is written in C++ and requires a C++ compiler to build. You will also need **CMake** to configure and build the project.

- C++ Compiler (e.g., GCC, Clang)
- CMake

### Building the Project

To build the project, follow these steps:

1. Create a build directory:

   ```bash
   mkdir build
   cd build
   ```

2. Configure the project using CMake:

   ```bash
   cmake ..
   ```

3. Build the project:

   ```bash
   make
   ```

### Running the Code

Once the build is complete, you can run the example program as follows:

```bash
./CLCStExample input_file.txt output_file.txt
```

- `input_file.txt`: Path to the input file containing two sequences.
- `output_file.txt`: Path to the output file where the result will be saved.

## Input Format

The input consists of two sequences, each on a separate line. The sequences are represented as strings of characters. The length `t` of substrings should be specified in the input or as a command-line argument.

Example input:

```
ATCGATCG
TGCATCGA
```

## Algorithm Overview

### Dynamic Programming for Subsequence Matching

The core of the algorithm is a dynamic programming approach that matches subsequences while considering the constraint of `t-length` substrings. Traditional LCS methods are adapted to ensure that only valid subsequences consisting of `t-length` substrings are considered. 

### Row-wise Optimization

To optimize the time complexity, the algorithm uses a row-wise approach to reduce the size of the problem at each step. By breaking down the matching process into smaller subproblems, the algorithm can process sequences more efficiently.

### Computational Complexity

- The time complexity of the algorithm is significantly reduced compared to brute-force solutions, making it feasible to handle large sequences and high values of `t`.
- The space complexity is optimized by using memory-efficient data structures and avoiding redundant computations.

## Example Usage

### Input File (`input_file.txt`)

```
ATCGATCG
TGCATCGA
```

### Output File (`output_file.txt`)

```
Longest Common Subsequence: ATCG
Length: 4
```

## Algorithm Details

The implementation is based on the following key principles:

1. **Dynamic Programming Matrix**: A matrix is constructed where each entry represents the length of the longest common subsequence for a particular substring combination.
2. **Backtracking**: Once the dynamic programming table is filled, the algorithm backtracks to recover the actual subsequence.
3. **Row-wise Optimization**: The table is filled row by row, optimizing the sequence matching process by reducing the computational overhead.

For a detailed explanation of the algorithms and their analysis, please refer to the original paper.

## Contributing

We welcome contributions! If you find any issues or have suggestions for improvements, feel free to open an issue or submit a pull request.

To contribute, follow these steps:

1. Fork the repository
2. Create a new branch
3. Implement your changes
4. Submit a pull request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- **Zhu, K., et al. (2021)** [email](zkxshg@gmail.com) for the original work and algorithms.
