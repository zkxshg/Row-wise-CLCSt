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

// LCSk in k length substring
static int k = 3;

// length of string A
static int m = 300;

// length of string B
static int n = 300;

// length of constrainred string P
static int r = 40;

// upbound of run_time
static int run_time = 100;

// ============================ DP-LCSt+ by Benson ============================
// Benson, G., et al. (2016). "LCSk: a refined similarity measure." Theoretical Computer Science 638: 11-26.

//Dynamic programming O(rmn)
int DP_Benson_Algorithm_plus(int* A, int* B, int m, int n, int k) {

    // 0 Initialize
    int** Benson1 = new int* [m];  // Benson LCSt+ matrix
    for (int i = 0; i < m; i++) Benson1[i] = new int[n];
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) Benson1[i][j] = 0;
    }

    int** kmatch1 = new int* [m];  // dcount matrix
    for (int i = 0; i < m; i++) kmatch1[i] = new int[n];
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) kmatch1[i][j] = 0;
    }

    // 1 Use kmatch1[i, j] to rec the continuous lenghs of common substrings in suffix of M[i][j] 
    // 1.1 boundary conditions
    for (int i = 0; i < m; i++) {
        if (A[i] == B[0]) kmatch1[i][0] = 1;
    }
    for (int i = 0; i < n; i++) {
        if (B[i] == A[0]) kmatch1[0][i] = 1;
    }

    // 1.2 calculate the dcount of M(i, j)
    for (int i = 1; i < m; i++) {
        for (int j = 1; j < n; j++) {
            if (A[i] == B[j]) kmatch1[i][j] = kmatch1[i - 1][j - 1] + 1;
        }
    }

    // 2 Use DP to get the LCSt+ of each element Benson1(m, n) depend on kmath1[i][j]
    // 2.1 boundary conditions
    for (int i = k - 1; i < m; i++) {
        if (kmatch1[i][k - 1] == k) Benson1[i][k - 1] = k;
        else Benson1[i][k - 1] = Benson1[i - 1][k - 1];
    }
    for (int i = k - 1; i < n; i++) {
        if (kmatch1[k - 1][i] == k) Benson1[k - 1][i] = k;
        else Benson1[k - 1][i] = Benson1[k - 1][i - 1];
    }

    // 2.2 Use DP and kmatch1 table to calculate the LCSt+(i, j)
    for (int i = k; i < m; i++) {
        for (int j = k; j < n; j++) {
            Benson1[i][j] = max(Benson1[i][j - 1], Benson1[i - 1][j]);
            for (int z = 0; z < k; z++) {
                if (kmatch1[i][j] >= k + z && i - k - z >= 0) 
                    Benson1[i][j] = max(Benson1[i - k - z][j - k - z] + k + z, Benson1[i][j]); // if (i, j) is a k-plus-match
            }
        }
    }

    int result = Benson1[m - 1][n - 1];  // M(m-1, n-1) = LCSt+(A, B)

    // 3 clear the memory
    for (int i = 0; i < m; i++) delete[] kmatch1[i];
    delete[] kmatch1;
    for (int i = 0; i < m; i++) delete[] Benson1[i];
    delete[] Benson1;

    return result;
}
