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

// ============================ sparse DP-LCSt+ by Pavetic ============================
// Pavetić, F., et al. (2017). "Fast and simple algorithms for computing both LCSk and LCSk+ ." arXiv preprint arXiv:1705.07279.
// ===== Row-wise match pair based: O(m + n + R + min(R(logl + k), R + ml))====
int Row_PV_plus_All_array(int* A, int* B, int m, int n, int k, int alphabet) {
    // ===== 0 创建后缀数组并计算LCP后分群 =====
    int* Z = new int[m + n + 1];
    for (int i = 0; i < m; i++) Z[i] = A[m - 1 - i];
    Z[m] = alphabet + 2;
    for (int j = 0; j < n; j++) Z[j + m + 1] = B[n - 1 - j];

    int* NewGroup = Group(Z, k, m, n);
    delete[] Z;

    int* groupA = new int[m];   // 将 group_all 还原为 A 和 B
    int* groupB = new int[n];

    for (int i = 0; i < m; i++) groupA[i] = NewGroup[m - 1 - i];
    for (int j = 0; j < n; j++) groupB[j] = NewGroup[m + n - j];
    delete[] NewGroup;

    // ===== 1 Initialize ===== 
    int PV = 0;

    int** StartPoint;  // 1.1 k-match 起点
    StartPoint = new int* [m];
    for (int i = 0; i < m; i++) StartPoint[i] = new int[n];

    int* StartPoint_num = new int[m];
    for (int i = 0; i < m; i++) StartPoint_num[i] = 0;

    int** StartPointLength;
    StartPointLength = new int* [m];
    for (int i = 0; i < m; i++) StartPointLength[i] = new int[n];

    int* StartPointLength_num = new int[m];
    for (int i = 0; i < m; i++) StartPointLength_num[i] = 0;

    int** EndPoint;  // 1.2 k-match 终点
    EndPoint = new int* [m];
    for (int i = 0; i < m; i++) EndPoint[i] = new int[n];

    int* EndPoint_num = new int[m];
    for (int i = 0; i < m; i++) EndPoint_num[i] = 0;

    int** EndPointLength;
    EndPointLength = new int* [m];
    for (int i = 0; i < m; i++) EndPointLength[i] = new int[n];

    int* EndPointLength_num = new int[m];
    for (int i = 0; i < m; i++) EndPointLength_num[i] = 0;

    // ===== 2 将 A 和 B 按照 group 分组 =====
    // 2.1 储存每个 group 成员的 index 
    int** group;
    group = new int* [m + n + 1];
    for (int i = 0; i < m + n + 1; i++) group[i] = new int[n];

    // 2.2 每个 group 内的成员数量
    int* group_num = new int[m + n + 1];
    for (int i = 0; i < m + n + 1; i++) group_num[i] = 0;

    // 2.3 计算 group_num
    for (int j = 0; j < n; j++) {
        int b = groupB[j];
        if (j >= k - 1) group[b][group_num[b]] = j;
        group_num[b] = group_num[b] + 1;
    }

    // 2.4 对 A 中每个 suffix 判断是否存在同组的 B
    for (int i = k - 1; i < m; i++) {
        int len = group_num[groupA[i]];
        for (int z = 0; z < len; z++) {
            StartPoint[i - k + 1][StartPoint_num[i - k + 1]] = group[groupA[i]][z] - k + 1;  // 记录 A 所在 group 中每个成员的起点和终点的 Row / B[j] 的位置
            StartPoint_num[i - k + 1] = StartPoint_num[i - k + 1] + 1;

            EndPoint[i][EndPoint_num[i]] = group[groupA[i]][z];
            EndPoint_num[i] = EndPoint_num[i] + 1;
        }
    }

    // ===== 3 计算 match pair-based LCSt =====

    // 3.1 Initialize d_pv
    int* d_pv = new int[m + 1];  // d_pv 记录该行每个 LCSt[d] 的最小列数

    d_pv[0] = -1;
    for (int i = 1; i < m + 1; i++) d_pv[i] = n + 1;

    // 3.2 start row-wise update of d_pv
    int stime = 0;

    for (int i = 0; i < m; i++) {
        if (PV == 0) stime = 0;
        else stime = (int)(log(PV) / log(2));

        // 若该行起点较少，则选择 binary search
        if (StartPoint_num[i] * stime < PV) {
            for (int sp = 0; sp < StartPoint_num[i]; sp++) {
                // binary search 起点行数的右区间，并将左区间记为该起点的 LCSt 长度
                StartPointLength[i][StartPointLength_num[i]] = BinarySearch_1(d_pv, StartPoint[i][sp], 0, PV);

                StartPointLength_num[i] = StartPointLength_num[i] + 1;
            }
        }
        else {
            int SI = 0;
            // linear scan start point
            for (int sp = 0; sp < StartPoint_num[i]; sp++) {
                while (d_pv[SI] < StartPoint[i][sp]) SI = SI + 1;
                StartPointLength[i][StartPointLength_num[i]] = SI - 1;

                StartPointLength_num[i] = StartPointLength_num[i] + 1;
            }
        }
        // 基于起点长度更新终点长度
        for (int ep = 0; ep < EndPoint_num[i]; ep++) {
            // 查找上一行是否有 G 满足: P continues G
            int contiG = 0;
            if (i > 0 && EndPoint_num[i - 1] > 0) {
                int G = 0;
                while (G < EndPoint_num[i - 1] && EndPoint[i - 1][G] < EndPoint[i][ep]) {
                    //  P continues G
                    if (EndPoint[i - 1][G] + 1 == EndPoint[i][ep]) {
                        contiG = EndPointLength[i - 1][G];
                        break;
                    }
                    else G++;
                }
            }

            // 对 LCSt+ 问题，end_length = max(start_length + k, G_length + 1)
            int pv_max = StartPointLength[i - k + 1][ep] + k;
            pv_max = max(pv_max, contiG + 1);

            //cout << "pv_max = " << pv_max << " amd contiG = " << contiG << "in row " << i << " col " << EndPoint[i][ep] << endl;
            EndPointLength[i][EndPointLength_num[i]] = pv_max;
            EndPointLength_num[i] = EndPointLength_num[i] + 1;

            // update d_pv
            for (int i2 = pv_max; i2 > pv_max - k; i2--) d_pv[i2] = min(d_pv[i2], EndPoint[i][ep]); 

            // cout << "d_pv = "; for (int i2 = 0; i2 < m + 1; i2++) cout << d_pv[i2] << " "; cout << endl;
            // update LCSt
            if (pv_max > PV) PV = pv_max;
        }

    }
    //cout << "d_pv = "; for (int i2 = 0; i2 < m + 1; i2++) cout << d_pv[i2] << " "; cout << endl;

    delete[] groupA;
    delete[] groupB;

    delete[] StartPoint;
    delete[] StartPoint_num;

    delete[] StartPointLength;
    delete[] StartPointLength_num;

    delete[] EndPoint;
    delete[] EndPoint_num;

    delete[] EndPointLength;
    delete[] EndPointLength_num;

    delete[] d_pv;

    delete[] group;
    delete[] group_num;

    return PV;
}
