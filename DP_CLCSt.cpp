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
// 测试数据路径
// 1000 * 1000; |bet| = 2;
static string fileName1 = "runtime_1000_1000_2_2.csv";// m_n_k_alphabet 
static string fileName2 = "string_1000_1000_2_2.csv";
// 5000 * 5000; |bet| = 4;
static string fileName3 = "runtime_5000_5000_2_4.csv";// m_n_k_alphabet 
static string fileName4 = "string_5000_5000_2_4.csv";
// 5000 * 5000; |bet| = 8;
static string fileName5 = "runtime_5000_5000_2_8.csv";// m_n_k_alphabet 
static string fileName6 = "string_5000_5000_2_8.csv";

// alphabet size
static int alphabet = 4;
// LCSk in k length substring
static int k = 4;
// length of string A
static int m = 500;
// length of string B
static int n = 500;
// length of constrainred string P
static int r = 50;
// upbound of run_time
static int run_time = 100;

// ============================ DP-CLCSt ============================

// calculate the length of suffix of P_k in A_i
int maxSuf(int* A, int* P, int pk, int ai) {
    int suf = 0;
    for (int i = ai; i > ai - k; i--) {
        if (A[i] == P[pk]) {
            suf++;
            pk--;
            if (pk < 0) break;
        }
    }
    return suf;
}

//Dynamic programming O(mn)
int DP_CLCSt(int* A, int* B, int* P, int m, int n, int r, int k) {

    // 0 Initialize
    int*** Benson1 = new int** [r + 1];  // 3-D CLCSt matrix
    for (int i = 0; i <= r; i++) {
        Benson1[i] = new int* [m + 1];
        for (int j = 0; j <= m; j++) {
            Benson1[i][j] = new int[n + 1];
            for (int h = 0; h <= n; h++) Benson1[i][j][h] = 0;
        }
    }
    cout << "Initialize \n";
    
    // dcount matrix
    int** kmatch1 = new int* [m];  
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
    cout << "kmatch1 boundary conditions \n";
    
    // 2 Use DP to get the CLCSt of each element Benson1(m, n) depend on kmath1[i][j]
    // 2.1 boundary conditions
    // k > 0
    for (int i = 0; i < m + 1; i++) {
        for (int k = 1; k < r + 1; k++) Benson1[k][i][0] = INT_MIN;
    }
    for (int j = 0; j < n + 1; j++) {
        for (int k = 1; k < r + 1; k++) Benson1[k][0][j] = INT_MIN;
    }

    for (int i = k - 1; i < m; i++) {
        if (kmatch1[i][k - 1] == k) Benson1[0][i + 1][k] = 1;
        else Benson1[0][i + 1][k] = Benson1[0][i][k];
    }
    for (int i = k - 1; i < n; i++) {
        if (kmatch1[k - 1][i] == k) Benson1[0][k][i + 1] = 1;
        else Benson1[0][k][i + 1] = Benson1[0][k][i];
    }
    cout << "Benson1 boundary conditions \n";
    
    // 2.2 Use DP and kmatch1 table to calculate the LCSt(i, j)
    // k = 0
    for (int i = k + 1; i <= m; i++) {
        for (int j = k + 1; j <= n; j++) {
            Benson1[0][i][j] = max(Benson1[0][i][j - 1], Benson1[0][i - 1][j]);
            if (kmatch1[i - 1][j - 1] >= k) Benson1[0][i][j] = max(Benson1[0][i - k][j - k] + 1, Benson1[0][i][j]); // if (i, j) is a k-match
        }
    }
    // k > 0
    for (int h = 1; h <= r; h++) {
        for (int i = 1; i <= m; i++) {
            for (int j = 1; j <= n; j++) {
                Benson1[h][i][j] = max(Benson1[h][i][j - 1], Benson1[h][i - 1][j]);
                if (kmatch1[i - 1][j - 1] >= k) {
                    int maxSuff = maxSuf(A, P, h - 1, i - 1);
                    Benson1[h][i][j] = max(Benson1[h - maxSuff][i - k][j - k] + 1, Benson1[h][i][j]); // if (i, j) is a k-match
                }
            }
        }
    }
    cout << "DP \n";

    int result = Benson1[r][m][n];  // M(m-1, n-1) = LCSt(A, B)

    // 3 clear the memory
    // for (int i = 0; i < m; i++) delete[] kmatch1[i];
    delete[] kmatch1;
    // for (int i = 0; i < m; i++) delete[] Benson1[i];
    delete[] Benson1;

    return result;
}
