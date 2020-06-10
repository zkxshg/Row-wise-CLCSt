#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <windows.h>

using namespace std;

// ============================ 定义常量 ============================
// alphabet size
static int alphabet = 4;

// length of string A
static int m = 300;

// length of string B
static int n = 300;

// upbound of run_time
static int run_time = 100;

// ============================ DP-LCS ============================

//Dynamic programming O(mn)
int DP_LCS_Algorithm(int* A, int* B, int m, int n) {

    // 0 Initialize
    int** Benson1 = new int* [m];  // Benson LCSt matrix
    for (int i = 0; i < m; i++) {
        Benson1[i] = new int[n];
        for (int j = 0; j < n; j++) Benson1[i][j] = 0;
    } 

    // 2 boundary conditions
    for (int i = 0; i < m; i++) {
        if (A[i] == B[0]) Benson1[i][0] = 1;
        else if (i > 0) Benson1[i][0] = Benson1[i - 1][0];
    }

    for (int i = 0; i < n; i++) {
        if (A[0] == B[i]) Benson1[0][i] = 1;
        else if (i > 0)  Benson1[0][i] = Benson1[0][i - 1];
    }

    // 1.2 Use DP to calculate the LCS(i, j)
    for (int i = 1; i < m; i++) {
        for (int j = 1; j < n; j++) {
            Benson1[i][j] = max(Benson1[i][j - 1], Benson1[i - 1][j]);
            if (A[i] == B[j]) Benson1[i][j] = max(Benson1[i - 1][j - 1] + 1, Benson1[i][j]); // if (i, j) is a match
        }
    }

    int result = Benson1[m - 1][n - 1];  // M(m-1, n-1) = LCSt(A, B)

    // 3 clear the memory
    for (int i = 0; i < m; i++) delete[] Benson1[i];
    delete[] Benson1;

    return result;
}
