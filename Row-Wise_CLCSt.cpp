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
//static int alphabet = 3;

// LCSk in k length substring
static int k = 4;
//static int k = 2;

// length of string A
static int m = 600;
//static int m = 10;

// length of string B
static int n = 600;
// static int n = 11;

// length of constrainred string P
static int r = 60;
//static int r = 3;

// upbound of run_time
static int run_time = 100;

// ============================ 二分搜索 ============================
// a: array; v: target; s: start; e: end
int BinarySearch(vector<int>& a, int v, int s, int e) {
    int p = (e + s) / 2;
    if (v > a[p]) {
        if (p < e) {
            if (v < a[p + 1]) return p;
            else return BinarySearch(a, v, p + 1, e);
        }
        else return e;
    }
    else if (p > 0) return BinarySearch(a, v, s, p - 1);
    else return -1;
}
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
    // A structure to store suffixes and their indexes
    struct suffix * suffixes = new suffix[n];

    // Store suffixes and their indexes in an array of structures. 
    // The structure is needed to sort the suffixes alphabatically and maintain their old indexes while sorting. 
    for (int i = 0; i < n; i++) {
        suffixes[i].index = i;
        suffixes[i].rank[0] = A[i];
        suffixes[i].rank[1] = ((i + 1) < n) ? (A[i + 1]) : -1;
    }

    // Sort the suffixes using the comparison function defined above. 
    sort(suffixes, suffixes + n, cmp);

    // At his point, all suffixes are sorted according to first 2 characters.  
    // Let us sort suffixes according to first 4 characters, then first 8 and so on. 

    // This array is needed to get the index in suffixes[] from original index.
    // This mapping is needed to get next suffix.
    int * ind = new int[n];  

    // Radix sort of suffixes
    for (int k = 4; k < 2 * n; k = k * 2) {
        // Assigning rank and index values to first suffix 
        int rank = 0;
        int prev_rank = suffixes[0].rank[0];
        suffixes[0].rank[0] = rank;   
        ind[suffixes[0].index] = 0;

        // Assigning rank to suffixes 
        for (int i = 1; i < n; i++) {
            // If first rank and next ranks are same as that of previous suffix in array, assign the same new rank to this suffix 
            if (suffixes[i].rank[0] == prev_rank &&
                suffixes[i].rank[1] == suffixes[i - 1].rank[1]) {
                prev_rank = suffixes[i].rank[0];
                suffixes[i].rank[0] = rank;
            } else {
                // Otherwise increment rank and assign 
                prev_rank = suffixes[i].rank[0];
                suffixes[i].rank[0] = ++rank;
            }
            ind[suffixes[i].index] = i;
        }

        // Assign next rank to every suffix 
        for (int i = 0; i < n; i++) {
            int nextindex = suffixes[i].index + k / 2;
            suffixes[i].rank[1] = (nextindex < n) ?
                suffixes[ind[nextindex]].rank[0] : -1;
        }

        // Sort the suffixes according to first k characters 
        sort(suffixes, suffixes + n, cmp);
    }

    // Store indexes of all sorted suffixes in the suffix array 
    vector<int> suffixArr;
    for (int i = 0; i < n; i++) suffixArr.push_back(suffixes[i].index);

    delete[] ind;
    delete[] suffixes;
    // Return the suffix array 
    return  suffixArr;
}

// calculate the LCP
vector<int> kasai(int* A, vector<int> suffixArr) {
    int n = suffixArr.size();

    // To store LCP array 
    vector<int> lcp(n, 0);

    // An auxiliary array to store inverse of suffix array elements. 
    // For example if suffixArr[0] is 5, the invSuff[5] would store 0.  
    // This is used to get next suffix string from suffix array. 
    vector<int> invSuff(n, 0);

    // Fill values in invSuff[] 
    for (int i = 0; i < n; i++) invSuff[suffixArr[i]] = i;

    // Initialize length of previous LCP 
    int k = 0;

    // Process all suffixes one by one starting from first suffix in txt[] 

    // If the current suffix is at n-1, then we don’t have next substring to consider. 
    // So lcp is not defined for this substring, we put zero.
    for (int i = 0; i < n; i++) {
        if (invSuff[i] == n - 1) {
            k = 0;
            continue;
        }

        // j contains index of the next substring to be considered to compare with the present substring, 
        // i.e., next string in suffix array.
        int j = suffixArr[invSuff[i] + 1];

        // Directly start matching from k'th index as at-least k-1 characters will match 
        while (i + k < n && j + k < n && A[i + k] == A[j + k]) k++;

        lcp[invSuff[i]] = k; // lcp for the present suffix. 

        // Deleting the starting character from the string. 
        if (k > 0) k--;
    }

    // return the constructed lcp array 
    return lcp;
}

// print an array 
void printArr(vector<int>arr, int n) {
    for (int i = 0; i < n; i++) cout << arr[i] << " ";
    cout << endl;
}

int* Group(int* T, int k, int m, int n) {
    // 创建后缀数组
    vector<int> suffixArr = buildSuffixArray(T, m + n + 1);
    int Sn = suffixArr.size();
    // cout << "Suffix Array : \n"; 
    // printArr(suffixArr, Sn);

    // 计算 LCP
    vector<int>lcp = kasai(T, suffixArr);
    // cout << "\nLCP Array : \n"; 
    // printArr(lcp, Sn); 

    // 记录所有 LCP > k 的 index
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
    /*cout << "\n group_all : \n";
    for (int i = 0; i < m + n + 1; i++) cout << group_all[i] << ", ";
    cout << endl;*/

    return group_all;
}

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

// ============================ Row-wise-CLCSt ============================

int Row_CLCSt(int* A, int* B, int* P, int m, int n, int r, int k) {

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

    for (int i = 0; i < m; i++) groupA[i] = NewGroup[m - 1 - i];
    for (int j = 0; j < n; j++) groupB[j] = NewGroup[m + n - j];
    delete[] NewGroup;

    // ===== 1 Initialize ===== 
    int * PV = new int[r + 1];  // 记录最后结果
    for (int i = 0; i < r + 1; i++) PV[i] = 0;  // PV[i] 记录每层最大长度

    // 1.1 k-match 起点
    int** StartPoint;
    StartPoint = new int* [m];
    for (int i = 0; i < m; i++) StartPoint[i] = new int[n];

    int* StartPoint_num = new int[m];
    for (int i = 0; i < m; i++) StartPoint_num[i] = 0;

    int*** StartPointLength = new int** [r + 1];
    for (int h = 0; h < r + 1; h++) {
        StartPointLength[h] = new int* [m];
        for (int i = 0; i < m; i++) StartPointLength[h][i] = new int[n];
    }

    int ** StartPointLength_num = new int * [r + 1];
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

    int*** EndPointLength = new int ** [r + 1];
    for (int h = 0; h < r + 1; h++) {
        EndPointLength[h] = new int * [m];
        for (int i = 0; i < m; i++) EndPointLength[h][i] = new int[n];
    }

    int** EndPointLength_num = new int* [r + 1];
    for (int h = 0; h < r + 1; h++) {
        EndPointLength_num[h] = new int[m];
        for (int i = 0; i < m; i++) EndPointLength_num[h][i] = 0;
    }

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

    // 2.4 对 A 中每个 suffix 判断是否存在同组的 B, 将同组 A,B 记录为 Match Pair
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

    // ===== 3 计算 match pair-based CLCSt =====
    // 3.1 Initialize N_pk
    int** N_pk = new int * [r + 1];  // N_pk[L, t] 记录在第 t 层该行每个 CLCSt = L 的最小列数

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
    for (int i = 0; i < m; i++)  {
        // 0.2 创建暂存的 N_pk table
        vector<vector<int>> tempnpk(r + 1);
        for (int i = 0; i < r + 1; i++) {
            for (int j = 0; j < m + 1; j++) tempnpk[i].push_back(N_pk[i][j]);
        }

        // 1 逐层计算
        for (int kc = 0; kc < r + 1; kc++) {
            // 2 计算起点在该行的 match pair 的 maxSuffix, k = 0 时为零
            int maxSuff;
            if (kc > 0) maxSuff = kc - maxSuf(A, P, kc - 1, i + k - 1);
            else maxSuff = 0;
            // cout << "maxSuff = " << maxSuff << " in layer " << kc << " and row " << i <<endl;

            // 3 根据已有 CLCk 长度计算平均搜索次数 ================OK=============
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

                    if (SI == 0 && maxSuff > 0)  StartPointLength[kc][i][StartPointLength_num[kc][i]] = -1;
                    else StartPointLength[kc][i][StartPointLength_num[kc][i]] = SI;
                    // cout << "SI = " << SI << " in maxSuff " << maxSuff << " and layer " << kc << " and row " << i << endl;
                   
                    StartPointLength_num[kc][i] = StartPointLength_num[kc][i] + 1;
                } 
            } else {
                // 4.2 该行起点较多，选择 linear scan
                int SI = 0;
                // 5,2 linear scan start point
                for (int sp = 0; sp < StartPoint_num[i]; sp++) {
                    while (tempnpk[maxSuff][SI] < StartPoint[i][sp]) SI = SI + 1;

                    if (SI == 1 && maxSuff > 0)  StartPointLength[kc][i][StartPointLength_num[kc][i]] = -1;
                    else StartPointLength[kc][i][StartPointLength_num[kc][i]] = SI - 1;
                    // cout << "SI = " << SI - 1 << " in maxSuff " << maxSuff << " and layer " << kc << " and row " << i << endl;

                    StartPointLength_num[kc][i] = StartPointLength_num[kc][i] + 1;
                }
            }
            /*cout << " length of start point in row " << i << " in layer " << kc << " is " << endl;
            for (int i2 = 0; i2 < StartPoint_num[i]; i2++) cout << StartPointLength[kc][i][i2] << " ";
            cout << endl;*/

            // 6 基于起点长度更新终点长度
            for (int ep = 0; ep < EndPoint_num[i]; ep++) {
                // 对 CLCSt 问题，end_length = start_length + 1
                int pv_max = StartPointLength[kc][i - k + 1][ep] + 1;

                EndPointLength[kc][i][EndPointLength_num[kc][i]] = pv_max;
                EndPointLength_num[kc][i] = EndPointLength_num[kc][i] + 1;

                // update N_pk
                if (pv_max > 0) {
                    N_pk[kc][pv_max] = min(EndPoint[i][ep], N_pk[kc][pv_max]);
                    for (int i2 = 1; i2 < pv_max; i2++) N_pk[kc][i2] = min(N_pk[kc][i2], N_pk[kc][pv_max]);
                }
                /*cout << " length of end point in row " << i << " in layer " << kc << " is " << endl;
                for (int i2 = 0; i2 < EndPoint_num[i]; i2++) cout << EndPointLength[kc][i][i2] << " ";
                cout << endl;*/

                // update LCSt
                if (pv_max > PV[kc]) {
                    PV[kc] = pv_max;
                    // cout << "update of PV_max = " << pv_max << "in layer " << kc << " and row " << i << endl;
                }

                /*cout << "N_pk = ";
                for (int i2 = 0; i2 < m + 1; i2++) cout << N_pk[kc][i2] << " ";
                cout << endl;*/
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
// ==================== 主测试函数 ====================
int main() {
    // 随机生成A, B序列
    int* A = new int[m];
    int* B = new int[n];
    int* P = new int[r];
    for (int i = 0; i < m; i++) A[i] = rand() % alphabet + 1;
    for (int i = 0; i < n; i++) B[i] = rand() % alphabet + 1;
    for (int i = 0; i < r; i++) P[i] = rand() % alphabet + 1;
    /*int* A = new int[10]{ 1, 1, 3, 2, 3, 3, 1, 3, 2, 1 };
    int* B = new int[11]{ 1, 1, 3, 3, 3, 1, 3, 2, 3, 2, 1 };
    int* P = new int[3]{3, 2, 2};*/



    cout << " A = ";
    for (int i = 0; i < m; i++) cout << A[i] << ",";
    cout << "\n B = ";
    for (int i = 0; i < n; i++) cout << B[i] << ",";
    cout << "\n P = ";
    for (int i = 0; i < r; i++) cout << P[i] << ",";
    cout << "\n";

    //int** Benson;//Benson
    //Benson = new int* [m];
    //for (int i = 0; i < m; i++) Benson[i] = new int[n];

    //int** kmatch;//kmatch
    //kmatch = new int* [m];
    //for (int i = 0; i < m; i++) kmatch[i] = new int[n];

    int BS = Benson_Algorithm(A, B, m, n, k);
    cout << "BS: " << BS << endl;

    int PV = PV_Method_All_array(A, B, m, n, k, alphabet);
    cout << "PV: " << PV << endl;

    int DP = DP_Chin(A, B, P, m, n, r);
    cout << "DP-CLCS: " << DP << endl;
    
    int CLCSt = DP_CLCSt(A, B, P, m, n, r, k);
    cout << "DP-CLCSt: " << CLCSt << endl;

    int RowCLCSt = Row_CLCSt(A, B, P, m, n, r, k);
    cout << "Row_CLCSt: " << RowCLCSt << endl;

    system("pause");
}
