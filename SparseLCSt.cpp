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
// 测试数据路径: A长度_B长度_子字串长度k_字符集大小
// 1000 * 1000; |bet| = 2;
static string fileName1 = "runtime_1000_1000_2_2.csv";// m_n_k_alphabet 
static string fileName2 = "string_1000_1000_2_2.csv";
// 5000 * 5000; |bet| = 4;
static string fileName3 = "runtime_5000_5000_2_4.csv";// m_n_k_alphabet 
static string fileName4 = "string_5000_5000_2_4.csv";
// 5000 * 5000; |bet| = 8;
static string fileName5 = "runtime_5000_5000_2_8.csv";// m_n_k_alphabet 
static string fileName6 = "string_5000_5000_2_8.csv";

// alphabet size 字符集大小
static int alphabet = 4;
// LCSk in k length substring 子字串长度k/t
static int k = 3;
// length of string A 
static int m = 1000;
// length of string B 
static int n = 1000;
// upbound of run_time
static int run_time = 100;

// ==================== DP-LCSt by Benson 动态规划法 ====================
// Benson, G., et al. (2016). "LCSk: a refined similarity measure." Theoretical Computer Science 638: 11-26.
// Dynamic programming O(mn)
int Benson_Algorithm(int* A, int* B, int m, int n, int k) {

    // ====== 0 Initialize ======
    int** Benson1 = new int* [m];  // Benson LCSt matrix
    for (int i = 0; i < m; i++) Benson1[i] = new int[n];
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) Benson1[i][j] = 0;
    }
    int** kmatch1 = new int* [m];  // dcount matrix
    for (int i = 0; i < m; i++) kmatch1[i] = new int[n];
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) kmatch1[i][j] = 0;
    }

    // ====== 1 Use kmatch1[i, j] to rec the continuous lenghs of common substrings in suffix of M[i][j] ====== 
    // ====== 1.1 boundary conditions ======
    for (int i = 0; i < m; i++) {
        if (A[i] == B[0]) kmatch1[i][0] = 1;
    }
    for (int i = 0; i < n; i++) {
        if (B[i] == A[0]) kmatch1[0][i] = 1;
    }
    // ====== 1.2 calculate the dcount of M(i, j) ======
    for (int i = 1; i < m; i++) {
        for (int j = 1; j < n; j++) {
            if (A[i] == B[j]) kmatch1[i][j] = kmatch1[i - 1][j - 1] + 1;
        }
    }

    // 2 ====== Use DP to get the LCSt of each element Benson1(m, n) depend on kmath1[i][j] ======
    // ====== 2.1 boundary conditions ======
    for (int i = k - 1; i < m; i++) {
        if (kmatch1[i][k - 1] == k) Benson1[i][k - 1] = 1;
        else Benson1[i][k - 1] = Benson1[i - 1][k - 1];
    }
    for (int i = k - 1; i < n; i++) {
        if (kmatch1[k - 1][i] == k) Benson1[k - 1][i] = 1; 
        else Benson1[k - 1][i] = Benson1[k - 1][i - 1];
    }
    // ====== 2.2 Use DP and kmatch1 table to calculate the LCSt(i, j) ======
    for (int i = k; i < m; i++) {
        for (int j = k; j < n; j++) {
            Benson1[i][j] = max(Benson1[i][j - 1], Benson1[i - 1][j]);
            if (kmatch1[i][j] >= k) Benson1[i][j] = max(Benson1[i - k][j - k] + 1, Benson1[i][j]); // if (i, j) is a k-match
        }
    }
    
    int result = Benson1[m - 1][n - 1];  // M(m-1, n-1) = LCSt(A, B)
    
    // ====== 3 clear the memory ======
    for (int i = 0; i < m; i++) delete[] kmatch1[i];
    delete[] kmatch1;
    for (int i = 0; i < m; i++) delete[] Benson1[i]; 
    delete[] Benson1;
    
    return result;
}

// ==================== 主测试函数 ====================
int main()
{
    // 随机生成A, B序列
    int* A = new int[m];
    int* B = new int[n];
    for (int i = 0; i < m; i++) A[i] = rand() % alphabet + 1;
    for (int i = 0; i < n; i++) B[i] = rand() % alphabet + 1;

    int** Benson;//Benson
    Benson = new int* [m];
    for (int i = 0; i < m; i++) Benson[i] = new int[n];

    int** kmatch;//kmatch
    kmatch = new int* [m];
    for (int i = 0; i < m; i++) kmatch[i] = new int[n];

    int BS = Benson_Algorithm(A, B, m, n, k);
    cout << "BS: " << BS << endl;
    // int PV = PV_Method_All_array(A, B, m, n, k, alphabet);
    // cout << "PV: " << PV << endl;
    
    system("pause");
}
