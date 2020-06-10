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
//// 测试数据路径
//// 1000 * 1000; |bet| = 2;
//static string fileName1 = "runtime_1000_1000_2_2.csv";// m_n_k_alphabet 
//static string fileName2 = "string_1000_1000_2_2.csv";
//// 5000 * 5000; |bet| = 4;
//static string fileName3 = "runtime_5000_5000_2_4.csv";// m_n_k_alphabet 
//static string fileName4 = "string_5000_5000_2_4.csv";
//// 5000 * 5000; |bet| = 8;
//static string fileName5 = "runtime_5000_5000_2_8.csv";// m_n_k_alphabet 
//static string fileName6 = "string_5000_5000_2_8.csv";

// alphabet size
static int alphabet = 4;

// LCSk in k length substring
static int k = 3;

// length of string A
static int m = 500;

// length of string B
static int n = 500;

// length of constrainred string P
static int r = 50;

// upbound of run_time
static int run_time = 100;
 
 // ============================ Row-wise-CLCSt+ ============================

 int Row_CLCSt_plus (int* A, int* B, int* P, int m, int n, int r, int k) {

     // ===== 0 创建后缀数组并计算LCP后分群 =====
     int* Z = new int[m + n + 1];  // 连接 A + |bet| + B = Z
     for (int i = 0; i < m; i++) Z[i] = A[m - 1 - i];
     Z[m] = alphabet + 2;
     for (int j = 0; j < n; j++) Z[j + m + 1] = B[n - 1 - j];

     int* NewGroup = Group(Z, k, m, n);
     delete[] Z;

     // 将 group_all 还原为 A 和 B
     int* groupA = new int[m];
     int* groupB = new int[n];

     for (int i = 0; i < m; i++) groupA[i] = NewGroup[m - 1 - i];  // Z[m - 1, 0] -> A[0, m - 1]
     for (int j = 0; j < n; j++) groupB[j] = NewGroup[m + n - j];  // Z[m + n, m + 1] -> B[0, n - 1]
     delete[] NewGroup;

     // ===== 1 Initialize ===== 
     int* PV = new int[r + 1];  // 记录最后结果
     for (int i = 0; i < r + 1; i++) PV[i] = 0;  // PV[i] 记录每层最大长度

     // 1.1 k-match 起点
     int** StartPoint;
     StartPoint = new int* [m];
     for (int i = 0; i < m; i++) StartPoint[i] = new int[n];

     int* StartPoint_num = new int[m];
     for (int i = 0; i < m; i++) StartPoint_num[i] = 0;

     int*** StartPointLength = new int** [r + 1]; // 记录每层起点长度: (r + 1) * m * n
     for (int h = 0; h < r + 1; h++) {
         StartPointLength[h] = new int* [m];
         for (int i = 0; i < m; i++) StartPointLength[h][i] = new int[n];
     }

     int** StartPointLength_num = new int* [r + 1]; // 记录每层起点个数: (r + 1) * m
     for (int h = 0; h < r + 1; h++) {
         StartPointLength_num[h] = new int[m];
         for (int i = 0; i < m; i++) StartPointLength_num[h][i] = 0;
     }

     // 1.2 k-match 终点
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
             StartPoint[i - k + 1][StartPoint_num[i - k + 1]] = group[groupA[i]][z] - k + 1;
             StartPoint_num[i - k + 1] = StartPoint_num[i - k + 1] + 1;

             EndPoint[i][EndPoint_num[i]] = group[groupA[i]][z];
             EndPoint_num[i] = EndPoint_num[i] + 1;
         }
     }

     // ===== 3 计算 match pair-based CLCSt+ =====
     // 3.1 Initialize N_pk
     int** N_pk = new int* [r + 1];  // N_pk[L, t] 记录在第 t 层该行每个 CLCSt = L 的最小列数

     // boundary condition
     // k = 0
     N_pk[0] = new int[m + 1];
     N_pk[0][0] = -1;
     for (int j = 1; j < m + 1; j++) N_pk[0][j] = n + 1;

     // k > 0
     for (int i = 1; i < r + 1; i++) {
         N_pk[i] = new int[m + 1];
         N_pk[i][0] = -1;
         for (int j = 1; j < m + 1; j++) N_pk[i][j] = n + 1;
     }

     // 3.2 start row-wise update of N_pk
     int stime = 0;

     // 0 逐行 update
     for (int i = 0; i < m; i++) {

         // 0.2 创建暂存的 N_pk table
         vector<vector<int>> tempnpk(r + 1);
         for (int i2 = 0; i2 < r + 1; i2++) {

             for (int j = 0; j < m + 1; j++) {
                 int temp = N_pk[i2][j];
                 tempnpk[i2].push_back(temp);
             }
         }

         // 1 逐层计算
         for (int kc = 0; kc < r + 1; kc++) {
             // 2 计算起点在该行的 match pair 的 maxSuffix, k = 0 时为零
             int maxSuff;
             if (kc > 0) maxSuff = kc - maxSuf(A, P, kc - 1, i + k - 1);
             else maxSuff = 0;
             // cout << "maxSuff = " << maxSuff << " in layer " << kc << " and row " << i <<endl;

             // 3 根据已有 CLCt+ 长度计算平均搜索次数 ================OK=============
             if (PV[maxSuff] == 0) stime = 0;
             else stime = (int)(log(PV[maxSuff]) / log(2));

             // 4 根据该行起点数比较 binary search 和 Linear Scan 效率
             // if (false) {
             if (StartPoint_num[i] * stime < PV[maxSuff]) {
                 // 4.1 binary search when total search time less than CLCSt in theory 

                 // 5.1 检查每个 start point
                 for (int sp = 0; sp < StartPoint_num[i]; sp++) {
                     // binary search 起点行数的右区间，并将左区间记为该起点的 LCSt 长度
                     int SI = BinarySearch(tempnpk[maxSuff], StartPoint[i][sp], 0, PV[maxSuff]);

                     if (SI == 0 && maxSuff > 0)  StartPointLength[kc][i][StartPointLength_num[kc][i]] = -1; // k > 0 and startLength = 0
                     else StartPointLength[kc][i][StartPointLength_num[kc][i]] = SI;

                     // cout << "SI = " << SI << " in maxSuff " << maxSuff << " and layer " << kc << " and row " << i << endl;

                     StartPointLength_num[kc][i] = StartPointLength_num[kc][i] + 1;
                 }
             }
             else {
                 // 4.2 该行起点较多，选择 linear scan
                 int SI = 0;
                 // 5,2 linear scan start point
                 for (int sp = 0; sp < StartPoint_num[i]; sp++) {
                     while (tempnpk[maxSuff][SI] < StartPoint[i][sp]) SI = SI + 1;

                     if (SI == 1 && maxSuff > 0)  StartPointLength[kc][i][StartPointLength_num[kc][i]] = -1;  // k > 0 and startLength = 0
                     else StartPointLength[kc][i][StartPointLength_num[kc][i]] = SI - 1;

                     //cout << "SI = " << SI - 1 << " in maxSuff " << maxSuff << " and layer " << kc << " and row " << i << endl;

                     StartPointLength_num[kc][i] = StartPointLength_num[kc][i] + 1;
                 }
             }
             //cout << " length of start point in row " << i << " in layer " << kc << " is " << endl;
             // for (int i2 = 0; i2 < StartPoint_num[i]; i2++) cout << StartPointLength[kc][i][i2] << " "; cout << endl;

             // 6 基于起点长度更新终点长度
             for (int ep = 0; ep < EndPoint_num[i]; ep++) {
                 // 对 CLCSt+ 问题，end_length = start_length + k
                 int pv_max = StartPointLength[kc][i - k + 1][ep] + k;

                 // 查找上一行是否有 G 满足: P continues G
                 int contiG = 0;
                 if (i > 0 && EndPoint_num[i - 1] > 0) {
                     int G = 0;
                     while (G < EndPoint_num[i - 1] && EndPoint[i - 1][G] < EndPoint[i][ep]) {
                         //  P continues G
                         if (EndPoint[i - 1][G] + 1 == EndPoint[i][ep]) {
                             if (kc > 0 && A[i] == P[kc - 1]) contiG = EndPointLength[kc - 1][i - 1][G];
                             else contiG = EndPointLength[kc][i - 1][G];
                             break;
                         }
                         else G++;
                     }
                 }
                 pv_max = max(pv_max, contiG + 1);
                 EndPointLength[kc][i][EndPointLength_num[kc][i]] = pv_max;
                 EndPointLength_num[kc][i] = EndPointLength_num[kc][i] + 1;

                 // update N_pk
                 if (pv_max > 0) {
                     N_pk[kc][pv_max] = min(EndPoint[i][ep], N_pk[kc][pv_max]);

                     for (int i2 = 1; i2 < pv_max; i2++) N_pk[kc][i2] = min(N_pk[kc][i2], N_pk[kc][pv_max]);
                 }
                 //cout << " length of end point in row " << i << " in layer " << kc << " is " << endl;
                 // for (int i2 = 0; i2 < EndPoint_num[i]; i2++) cout << EndPointLength[kc][i][i2] << " "; cout << endl;

                 // update LCSt
                 if (pv_max > PV[kc]) {
                     PV[kc] = pv_max;
                     //cout << "update of PV_max = " << pv_max << "in layer " << kc << " and row " << i << endl;
                 }

                 //cout << "N_pk = "; for (int i2 = 0; i2 < m + 1; i2++) cout << N_pk[kc][i2] << " "; cout << endl;
             }
         }
     }

     delete[] groupA;
     delete[] groupB;
     delete[] StartPoint_num;
     delete[] StartPointLength_num;
     delete[] EndPointLength_num;
     delete[] EndPoint_num;
     delete[] N_pk;
     delete[] group_num;

     return PV[r];
 }
