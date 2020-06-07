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

// ============================ 二分搜索 ============================
// a: array; v: target; s: start; e: end
// binary search for int * 
int BinarySearch_1(int* a, int v, int s, int e) {
    int p = (e + s) / 2;
    if (v > a[p]) {
        if (p < e) {
            if (v < a[p + 1]) return p;
            else return BinarySearch_1(a, v, p + 1, e);
        }
        else return e;
    }
    else if (p > 0) return BinarySearch_1(a, v, s, p - 1);
    else return -1;
}

// ============================ 构建后缀数组 ============================
// 后缀结构
struct suffix {
    int index;  // To store original index 
    int rank[2]; // To store ranks and next rank pair 
};

// 基于字典序比较 suffix 大小：1: a < b; 0: a > b 
int cmp(struct suffix a, struct suffix b) {
    return (a.rank[0] == b.rank[0]) ? (a.rank[1] < b.rank[1] ? 1 : 0) :
        (a.rank[0] < b.rank[0] ? 1 : 0);
}

// 创建后缀数组
vector<int> buildSuffixArray(int* A, int n) {
    struct suffix * suffixes = new suffix[n];
    for (int i = 0; i < n; i++) {
        suffixes[i].index = i;
        suffixes[i].rank[0] = A[i];
        suffixes[i].rank[1] = ((i + 1) < n) ? (A[i + 1]) : -1;
    }
    sort(suffixes, suffixes + n, cmp);
    int * ind = new int[n];  
    for (int k = 4; k < 2 * n; k = k * 2) {
        int rank = 0;
        int prev_rank = suffixes[0].rank[0];
        suffixes[0].rank[0] = rank;   
        ind[suffixes[0].index] = 0;
        for (int i = 1; i < n; i++) {
            if (suffixes[i].rank[0] == prev_rank &&
                suffixes[i].rank[1] == suffixes[i - 1].rank[1]) {
                prev_rank = suffixes[i].rank[0];
                suffixes[i].rank[0] = rank;
            } else {
                prev_rank = suffixes[i].rank[0];
                suffixes[i].rank[0] = ++rank;
            }
            ind[suffixes[i].index] = i;
        }
        for (int i = 0; i < n; i++) {
            int nextindex = suffixes[i].index + k / 2;
            suffixes[i].rank[1] = (nextindex < n) ?
                suffixes[ind[nextindex]].rank[0] : -1;
        }
        sort(suffixes, suffixes + n, cmp);
    }
    vector<int> suffixArr;
    for (int i = 0; i < n; i++) suffixArr.push_back(suffixes[i].index);
    delete[] ind;
    delete[] suffixes;
    return  suffixArr;
}

vector<int> kasai(int* A, vector<int> suffixArr) {
    int n = suffixArr.size();
    vector<int> lcp(n, 0);
    vector<int> invSuff(n, 0);
    for (int i = 0; i < n; i++) invSuff[suffixArr[i]] = i;
    int k = 0;
    for (int i = 0; i < n; i++) {
        if (invSuff[i] == n - 1) {
            k = 0;
            continue;
        }
        int j = suffixArr[invSuff[i] + 1];
        while (i + k < n && j + k < n && A[i + k] == A[j + k]) k++;
        lcp[invSuff[i]] = k; // lcp for the present suffix. 
        if (k > 0) k--;
    }
    return lcp;
}

void printArr(vector<int>arr, int n) {
    for (int i = 0; i < n; i++) cout << arr[i] << " ";
    cout << endl;
}

int* Group(int* T, int k, int m, int n) {
    vector<int> suffixArr = buildSuffixArray(T, m + n + 1);
    int Sn = suffixArr.size();
    vector<int>lcp = kasai(T, suffixArr);
    int* group_all = new int[m + n + 1];
    for (int i = 0; i < m + n + 1; i++) group_all[i] = 0;
    int index = 1;
    group_all[suffixArr[0]] = 1;
    for (int i = 0; i < m + n; i++) {
        if (lcp[i] >= k) group_all[suffixArr[i + 1]] = index;
        else {
            index = index + 1;
            group_all[suffixArr[i + 1]] = index;/* code */
        }
    }
    return group_all;
}

int Row_CLCSt(int* A, int* B, int* P, int m, int n, int r, int k) {

    // ===== 0 创建后缀数组并计算LCP后分群 =====
    // 连接 A + |bet| + B = Z
    int* Z = new int[m + n + 1];
    for (int i = 0; i < m; i++) Z[i] = A[m - 1 - i];
    Z[m] = alphabet + 2;
    for (int j = 0; j < n; j++) Z[j + m + 1] = B[n - 1 - j];

    int* NewGroup = Group(Z, k, m, n);
    delete[] Z;

    cout << "After Group" << endl;

    // 将 group_all 还原为 A 和 B
    int* groupA = new int[m];
    int* groupB = new int[n];

    for (int i = 0; i < m; i++) groupA[i] = NewGroup[m - 1 - i];
    // for (int i = 0; i < m; i++) cout << NewGroup[m - 1 - i] << " "; cout << endl;
    for (int j = 0; j < n; j++) groupB[j] = NewGroup[m + n - j];
    delete[] NewGroup;

    // ===== 1 Initialize ===== 
    // 记录最后结果
    // PV[i] 记录每层最大长度
    int * PV = new int[r + 1];
    for (int i = 0; i < r + 1; i++) PV[i] = 0;
    // 1.1 k-match 起点
    int** StartPoint;
    StartPoint = new int* [m];
    for (int i = 0; i < m; i++) StartPoint[i] = new int[n];

    int* StartPoint_num = new int[m];
    for (int i = 0; i < m; i++) StartPoint_num[i] = 0;

    int** StartPointLength;
    StartPointLength = new int* [m];
    for (int i = 0; i < m; i++) StartPointLength[i] = new int[n];

    int* StartPointLength_num = new int[m];
    for (int i = 0; i < m; i++) StartPointLength_num[i] = 0;

    // 1.2 k-match 终点
    int** EndPoint;
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

    // 2.4 对 A 中每个 suffix 判断是否存在同组的 B, 将同组记录为 Match Pair
    for (int i = k - 1; i < m; i++) {
        int len = group_num[groupA[i]];
        // 记录 A 所在 group 中每个成员的起点和终点的 Row / B[j] 的位置
        for (int z = 0; z < len; z++) {
            StartPoint[i - k + 1][StartPoint_num[i - k + 1]] = group[groupA[i]][z] - k + 1;
            StartPoint_num[i - k + 1] = StartPoint_num[i - k + 1] + 1;
            EndPoint[i][EndPoint_num[i]] = group[groupA[i]][z];
            EndPoint_num[i] = EndPoint_num[i] + 1;
        }
    }

    // ===== 3 计算 match pair-based LCSt =====
    // 3.1 Initialize N_pk

    // N_pk[L, t] 记录在第 t 层该行每个 CLCSt = L 的最小列数
    int** N_pk = new int * [r + 1];
    for (int i = 0; i < r + 1; i++) {
        N_pk[i] = new int[m + 1];
        N_pk[i][0] = -1;
        for (int j = 1; j < m + 1; j++) N_pk[i][j] = n + 1;
    }

    // 3.2 start row-wise update of N_pk

    // k = 0
    int stime = 0;
    for (int i = 0; i < m; i++) {
        if (PV[0] == 0) stime = 0;
        else stime = (int)(log(PV[0]) / log(2));

        // 若该行起点较少，则选择 binary search
        if (StartPoint_num[i] * stime < PV[0]) {
            // 检查每个 start point
            for (int sp = 0; sp < StartPoint_num[i]; sp++) {
                // binary search 起点行数的右区间，并将左区间记为该起点的 LCSt 长度
                StartPointLength[i][StartPointLength_num[i]] = BinarySearch_1(N_pk[0], StartPoint[i][sp], 0, PV[0]);
                StartPointLength_num[i] = StartPointLength_num[i] + 1;
            }
        }
        else {
            // 该行起点较多，选择 linear scan
            int SI = 0;
            // linear scan start point
            for (int sp = 0; sp < StartPoint_num[i]; sp++) {
                while (N_pk[0][SI] < StartPoint[i][sp]) SI = SI + 1;
                StartPointLength[i][StartPointLength_num[i]] = SI - 1;
                StartPointLength_num[i] = StartPointLength_num[i] + 1;
            }
        }
        // 基于起点长度更新终点长度
        for (int ep = 0; ep < EndPoint_num[i]; ep++) {
            // 对 LCSt 问题，end_length = start_length + 1
            int pv_max = StartPointLength[i - k + 1][ep] + 1;
            EndPointLength[i][EndPointLength_num[i]] = pv_max;
            EndPointLength_num[i] = EndPointLength_num[i] + 1;
            // update d_pv
            N_pk[0][pv_max] = min(EndPoint[i][ep], N_pk[0][pv_max]);
            // update LCSt
            if (pv_max > PV[0]) PV[0] = pv_max;
        }
    }
    // k > 0 [0]
    stime = 0;
    for (int kc = 1; kc < r + 1; kc++) {
        // 初始化 startLength_num 和 EndLength_num
        for (int j = 0; j < m; j++) {
            EndPointLength[j] = new int[n]; 
            StartPointLength[j] = new int[n];
            StartPointLength_num[j] = 0;
            EndPointLength_num[j] = 0;
        }
        // 逐行 update
        for (int i = 0; i < m; i++) {
            // 计算起点在该行的 match pair 的 maxSuffix
            int maxSuff = kc - maxSuf(A, P, kc - 1, i + k - 1);
            // cout << "maxSuff = " << maxSuff << "in layer " << kc << endl;
            // 根据已有 CLCk 长度计算平均搜索次数
            if (PV[kc] == 0) stime = 0;
            else stime = (int)(log(PV[kc]) / log(2));
            
            // 根据该行起点数比较 binary search 和 Linear Scan 效率
            if (StartPoint_num[i] * stime < PV[kc]) {
                // 检查每个 start point
                for (int sp = 0; sp < StartPoint_num[i]; sp++) {
                    // binary search 起点行数的右区间，并将左区间记为该起点的 LCSt 长度
                    StartPointLength[i][StartPointLength_num[i]] = BinarySearch_1(N_pk[maxSuff], StartPoint[i][sp], 0, PV[kc]);
                    StartPointLength_num[i] = StartPointLength_num[i] + 1;
                } 3

            } else {
                // 该行起点较多，选择 linear scan
                int SI = 0;
                // linear scan start point
                for (int sp = 0; sp < StartPoint_num[i]; sp++) {
                    while (N_pk[maxSuff][SI] < StartPoint[i][sp]) SI = SI + 1;
                    StartPointLength[i][StartPointLength_num[i]] = SI - 1;
                    StartPointLength_num[i] = StartPointLength_num[i] + 1;
                }
            }
            // 基于起点长度更新终点长度
            for (int ep = 0; ep < EndPoint_num[i]; ep++) {
                // 对 CLCSt 问题，end_length = start_length + 1
                int pv_max = StartPointLength[i - k + 1][ep] + 1;
                EndPointLength[i][EndPointLength_num[i]] = pv_max;
                EndPointLength_num[i] = EndPointLength_num[i] + 1;
                // update N_pk
                N_pk[kc][pv_max] = min(EndPoint[i][ep], N_pk[kc][pv_max]);
                // update LCSt
                if (pv_max > PV[kc]) {
                    PV[kc] = pv_max;
                    cout << "update of PV_max = " << pv_max << "in layer " << kc << " and row " << i << endl;
                }
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
