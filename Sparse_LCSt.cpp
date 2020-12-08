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

// ============================ 构建后缀数组 ============================
// 后缀结构
struct suffix {
    int index;  // To store original index 
    int rank[2]; // To store ranks and next rank pair 
};

// 基于字典序比较 suffix 大小：1: a < b; 0: a > b 
int cmp(struct suffix a, struct suffix b) {
    return (a.rank[0] == b.rank[0]) ? (a.rank[1] < b.rank[1] ? 1 : 0) : (a.rank[0] < b.rank[0] ? 1 : 0);
}

// 创建后缀数组
vector<int> buildSuffixArray(int* A, int n) {
    // A structure to store suffixes and their indexes 创建 struct
    struct suffix * suffixes = new suffix[n];

    // Store suffixes and their indexes in an array of structures. 
    // Index = i 的 rank[0] = A[i], rank[1] = A[i + 1]. 
    // The structure is needed to sort the suffixes alphabatically and maintain their old indexes while sorting. 
    for (int i = 0; i < n; i++) {
        suffixes[i].index = i;
        suffixes[i].rank[0] = A[i];
        suffixes[i].rank[1] = ((i + 1) < n) ? (A[i + 1]) : -1;
    }

    // Sort the suffixes using the comparison function defined above. 
    // 基于 [i, i+1] 排序
    sort(suffixes, suffixes + n, cmp);

    // At his point, all suffixes are sorted according to first 2 characters.  
    // Let us sort suffixes according to first 4 characters, then first 8 and so on. 

    // This array is needed to get the index in suffixes[] from original index.
    // This mapping is needed to get next suffix.
    int * ind = new int[n];  

    // Radix sort of suffixes. 
    // 基于 [u, i + k / 2], k = 4 to n, 进行 stable radix sort.
    for (int k = 4; k < 2 * n; k = k * 2) {
        // Assigning rank and index values to first suffix 
        int rank = 0;
        int prev_rank = suffixes[0].rank[0];
        suffixes[0].rank[0] = rank;   
        ind[suffixes[0].index] = 0;

        // Assigning rank to suffixes 
        for (int i = 1; i < n; i++) {
            // If first rank and next ranks are same as that of previous suffix in array, assign the same new rank to this suffix 
            if (suffixes[i].rank[0] == prev_rank && suffixes[i].rank[1] == suffixes[i - 1].rank[1]) {
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
            suffixes[i].rank[1] = (nextindex < n) ? suffixes[ind[nextindex]].rank[0] : -1;
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

    // 将所有 LCP > k 的 index 划为同一 group (match pairs)
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

// ==================== sparse DP-LCSt by Pavetic ====================
// Pavetić, F., et al. (2017). "Fast and simple algorithms for computing both LCSk and LCSk+ ." arXiv preprint arXiv:1705.07279.
// ===== Row-wise match pair based: O(m + n + R + min(Rlogl, R + ml))====

int PV_Method_All_array(int* A, int* B, int m, int n, int k, int alphabet) {
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
    int PV = 0;
    // 1.1 k-match 起点
    int** StartPoint;
    StartPoint = new int* [m];
    for (int i = 0; i < m; i++) StartPoint[i] = new int[n];

    int * StartPoint_num = new int[m];
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

    // 2.4 对 A 中每个 suffix 判断是否存在同组的 B
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
    // 3.1 Initialize d_pv
    // d_pv 记录该行每个 LCSt[d] 的最小列数
    int * d_pv = new int[m + 1];
    d_pv[0] = -1;
    for (int i = 1; i < m + 1; i++) d_pv[i] = n;
    
    // 3.2 start row-wise update of d_pv
    int stime = 0;

    for (int i = 0; i < m; i++) {
        if (PV == 0) stime = 0;
        else stime = (int)(log(PV) / log(2));

        // 若该行起点较少，则选择 binary search
        if (StartPoint_num[i] * stime < PV) {
            // 检查每个 start point
            for (int sp = 0; sp < StartPoint_num[i]; sp++) {   
                // binary search 起点行数的右区间，并将左区间记为该起点的 LCSt 长度
                StartPointLength[i][StartPointLength_num[i]] = BinarySearch_1(d_pv, StartPoint[i][sp], 0, PV);
                StartPointLength_num[i] = StartPointLength_num[i] + 1;
            }
        } else {
            // 该行起点较多，选择 linear scan
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
            // 对 LCSt 问题，end_length = start_length + 1
            int pv_max = StartPointLength[i - k + 1][ep] + 1;
            EndPointLength[i][EndPointLength_num[i]] = pv_max;
            EndPointLength_num[i] = EndPointLength_num[i] + 1;
            // update d_pv
            d_pv[pv_max] = min(EndPoint[i][ep], d_pv[pv_max]);
            // update LCSt
            if (pv_max > PV) PV = pv_max;
        }

    }
    delete[] groupA;
    delete[] groupB;
    delete[] StartPoint_num; 
    delete[] StartPointLength_num; 
    delete[] EndPointLength_num;
    delete[] EndPoint_num; 
    delete[] d_pv;
    delete[] group_num;

    return PV;
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
    
    int PV = PV_Method_All_array(A, B, m, n, k, alphabet);
    cout << "PV: " << PV << endl;
    
    system("pause");
}
