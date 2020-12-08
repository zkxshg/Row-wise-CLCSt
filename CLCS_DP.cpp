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
static int m = 100;
// length of string B
static int n = 100;
// length of constrainred string P
static int r = 5;
// upbound of run_time
static int run_time = 100;

// ============================  DP-CLCS by Chin ============================
// Chin, F. Y., et al. (2004). "A simple algorithm for the constrained sequence problems." Information Processing Letters 90(4): 175-179.
//===== Dynamic programming O(rmn) =====

template<typename T>
std::vector<std::vector<std::vector<T>>> make_3d_vector(int z, int y, int x, T value = T{}) {
    return std::vector<std::vector<std::vector<T>>>(z, std::vector<std::vector<T>>(y, std::vector<T>(x, value)));
}

/** Chin et al. algorithm */
int DP_Chin(int* A, int* B, int* P, int m, int n, int r) {
    // 0 save in 3D vector 
    auto M3d = make_3d_vector<int>(r + 1, m + 1, n + 1, 0);

    // 1 boundary conditions (initializations):
    // k = 0
    for (int i = 0; i < m + 1; i++) M3d[0][i][0] = 0;
    for (int j = 0; j < n + 1; j++) M3d[0][0][j] = 0;
    // k > 0
    for (int i = 0; i < m + 1; i++) {
        for (int k = 1; k < r + 1; k++) M3d[k][i][0] = INT_MIN;
    }
    for (int j = 0; j < n + 1; j++) {
        for (int k = 1; k < r + 1; k++) M3d[k][0][j] = INT_MIN;
    }

    // 2 execute the DP recursion: 
    for (int k = 0; k <= r; k++) {
        for (int i = 1; i <= m; i++) {
            for (int j = 1; j <= n; j++) {
                int lmax = INT_MIN;
                if (k > 0 && A[i - 1] == B[j - 1] && B[j - 1] == P[k - 1])
                    lmax = M3d[k - 1][i - 1][j - 1] + 1; // A[i] = B[j] = P[k]
                else if (A[i - 1] == B[j - 1])
                    lmax = M3d[k][i - 1][j - 1] + 1; // A[i] = B[j] != P[k] or k = 0
                lmax = max(M3d[k][i - 1][j], lmax);
                M3d[k][i][j] = max(M3d[k][i][j - 1], lmax);
            }
        }
    }
    // TODO: retrieving optimal solution
    int k = r, i = m, j = n, len = 0;
    cout << "optimal solution " << M3d[k][i][j] << " " << M3d[0][0][0] << endl;

    // 3 Backtracing
    vector<int> s; // result
    while (!(i == 0 and j == 0 and k == 0))
    {
        if ((i > 0 and j > 0 and k > 0) and A[i - 1] == B[j - 1] and B[j - 1] == P[k - 1]) {
            s.push_back(A[i - 1]); i--; j--; k--; len++;
        }
        else if ((i > 0 and j > 0) and A[i - 1] == B[j - 1] and (k == 0 or A[i - 1] != P[k - 1])) {
            s.push_back(A[i - 1]); i--; j--; len++;
        }
        else if (j > 0 and i > 0) {
            if (M3d[k][i - 1][j] > M3d[k][i][j - 1]) i--;
            else j--;
        }
        else break;
    }
    // reverse the vector to get the optimal sol
    std::reverse(s.begin(), s.end());
    // cout << "s = "; for (int i : s) cout << i << ", "; cout << endl;
    return M3d[r][m][n];
}

int main()
{
    // 随机生成A, B序列
    int* A = new int[m];
    int* B = new int[n];
    int* P = new int[r];
    for (int i = 0; i < m; i++) A[i] = rand() % alphabet + 1;
    for (int i = 0; i < n; i++) B[i] = rand() % alphabet + 1;
    for (int i = 0; i < r; i++) P[i] = rand() % alphabet + 1;

    int DP = DP_Chin(A, B, P, m, n, r);
    cout << "DP: " << DP << endl;
    system("pause");
    return 0;
}
