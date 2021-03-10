#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <climits>
#include <windows.h>

using namespace std;

/ ============================ Diagonal-LCSt+ by Huang ============================
// G.-F. Huang, "Diagonal Algorithms for the Longest Common Subsequence Problems with t-length and at Least t-length Substrings," 2020.
// ===== Diagonal O(n(m - L)) =====
int Diagonal_LCSt(int* A, int* B, int m, int n, int k) {
    // ===== 1 创建后缀数组并计算LCP后分群 =====

    // 1.1 连接 A + |bet| + B = Z 
    int* Z = new int[m + n + 1];
    for (int i = 0; i < m; i++) Z[i] = A[m - 1 - i];
    Z[m] = alphabet + 2;
    for (int j = 0; j < n; j++) Z[j + m + 1] = B[n - 1 - j];

    int* NewGroup = Group(Z, k, m, n);
    delete[] Z;

    // 1.2 将 group_all 还原为 A 和 B
    int* groupA = new int[m];
    int* groupB = new int[n];

    for (int i = 0; i < m; i++) groupA[i] = NewGroup[m - 1 - i]; // Z[m - 1, 0] -> A[0, m - 1]
    for (int j = 0; j < n; j++) groupB[j] = NewGroup[m + n - j]; // Z[m + n, m + 1] -> B[0, n - 1]
    delete[] NewGroup;

     // 1.3 记录k-match 终点
    int** EndPoint;
    EndPoint = new int* [m];
    for (int i = 0; i < m; i++) EndPoint[i] = new int[n];

    int* EndPoint_num = new int[m];
    for (int i = 0; i < m; i++) EndPoint_num[i] = 0;

    int*** EndPointLength = new int** [r + 1];   // 记录每层终点长度: (r + 1) * m * n
    for (int h = 0; h < r + 1; h++) {
        EndPointLength[h] = new int* [m];
        for (int i = 0; i < m; i++) EndPointLength[h][i] = new int[n];
    }

    int** EndPointLength_num = new int* [r + 1];  // 记录每层终点个数: (r + 1) * m
    for (int h = 0; h < r + 1; h++) {
        EndPointLength_num[h] = new int[m];
        for (int i = 0; i < m; i++) EndPointLength_num[h][i] = 0;
    }

    // ===== 2 将 A 和 B 按照 group 分组 =====
     // 2.1 储存每个 group 成员的 index: group (m + n + 1) * n
    int** group;
    group = new int* [m + n + 1];
    for (int i = 0; i < m + n + 1; i++) group[i] = new int[n];

    // 2.2 每个 group 内的成员数量: group_num( m + n + 1) * 1
    int* group_num = new int[m + n + 1];
    for (int i = 0; i < m + n + 1; i++) group_num[i] = 0;


    // 2.3 计算 B 所属的 group_num
    for (int j = 0; j < n; j++) {
        int b = groupB[j];
        if (j >= k - 1) group[b][group_num[b]] = j;
        group_num[b] = group_num[b] + 1;
    }

    // 2.4 对 A 中每个 suffix 判断是否存在同组的 B, 将同组 A,B 记录为 Match Pair
    for (int i = k - 1; i < m; i++) {
        int len = group_num[groupA[i]];
        // 记录 A 所在 group 中每个成员的起点和终点的 Row / B[j] 的位置:  start[A_index - k + 1][num] = B_index - k + 1; End[A_index][num] = B_index
        for (int z = 0; z < len; z++) {
            EndPoint[i][EndPoint_num[i]] = group[groupA[i]][z];
            EndPoint_num[i] = EndPoint_num[i] + 1;
        }
    }

    // 3 Diagonal LCSt
    // Initialize
    int nk = n / k + 1;
    int* dk_ij = new int[nk];
    for (int i = 0; i < nk; i++) dk_ij[i] = n * 10;

    // Start
    int maxL = 0;
    for (int i = k; i <= m; i++) {

        int pos = 0;
        for (int j = i; j <= m; j += k) {
            // search next k-match
            // int* nexPP = lower_bound(EndPoint[j], EndPoint[j] + EndPoint_num[j], pos);
            int nexP = n * 10;
            for (int en = 0; en < EndPoint_num[j]; en++) {
                if (EndPoint[j][en] >= pos + k - 1) {
                    nexP = EndPoint[j][en] + 1; break;
                }       
            }

            int minCol = min(dk_ij[(j - i) / k], nexP);
            if (minCol > n) break;

            dk_ij[(j - i) / k] = minCol;
            maxL = max(maxL, (j - i) / k + 1);
            pos = dk_ij[(j - i) / k];
        }

        if (m - i < maxL * k) break;
    }
    //for (int i = 0; i < maxL; i++) cout << dk_ij[i] << ","; cout << endl;

    delete[] dk_ij;

    delete_3D_Array(EndPointLength, r + 1, m, n);
    delete_2D_Array(EndPoint, m, n);
    delete_2D_Array(EndPointLength_num, r + 1, m);

    delete_2D_Array(group, m + n + 1, n);

    delete[] groupA;
    delete[] groupB;

    delete[] group_num;
    delete[] EndPoint_num;

    return maxL;
}
