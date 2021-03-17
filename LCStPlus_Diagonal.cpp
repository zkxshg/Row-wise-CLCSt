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
#include <unordered_map>
#include<map> 
#include <windows.h>
#define MaxValue 9999999

using namespace std;

// ====== 删除int * 中的某个元素 ======
void erase_array(int * arr, int len, int pos) {
    for (int i = pos; i < len - 1; i++) arr[i] = arr[i + 1];
}

// ============================ Diagonal-LCSt+ by Huang ============================
// ===== Diagonal O(L(m - L) - R) =====
int Diagonal_LCSt_plus(int* A, int* B, int m, int n, int k) {
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
    
    // 3 Diagonal LCSt+
    // Initialize
    int nk = n + 1;
    int* dk_ij = new int[nk]; dk_ij[0] = 0;
    for (int i = 1; i < nk; i++) dk_ij[i] = nk * 10;

    // Start
    int maxL = 0;
    // Round i: O(m - L)
    double t1 = 0.0, t2 = 0.0, t3 = 0.0;
    for (int i = k - 1; i < m; i++) {

        int pos = 0, couK = k;
        vector<int> nexM;
        // Row j: O(L)
        for (int j = i; j < m; j++) {
            pos = dk_ij[j - i];

            int nexP = n * 10;

            int nexE = n;          
            if (pos <= n) {
                // Search next Match: Total O(R)
                for (int en = 0; en < EndPoint_num[j]; en++) {
                    if (EndPoint[j][en] >= pos + k - 1) {
                        nexP = min(EndPoint[j][en] + 1, nexP); nexE = en; break;
                    }
                }    
                //int en = lower_bound(EndPoint[j], EndPoint[j] + EndPoint_num[j], pos + k - 1) - EndPoint[j];
                //if (en < EndPoint_num[j]) nexP = min(EndPoint[j][en] + 1, nexP); nexE = en;
            }
 
            // Search the nextM table
            if (!nexM.empty()) {
                int minC = n; 
                minC = *min_element(nexM.begin(), nexM.end());
                nexP = min(minC + 1, nexP);
            }

            // Search the extensions: Total O(R)
            vector<int> temM; 
            if (j + 1 < m) {
                // search last nextM table
                for (int nm : nexM) 
                    if (nm + 1 < n && A[j + 1] == B[nm + 1]) 
                        temM.push_back(nm + 1); 

                // search rest Match pairs: Total O(R)
                vector<int> era_node;
                for (int en = nexE; en < EndPoint_num[j]; en++) {
                    int remE = EndPoint[j][en];
                    if (remE + 1 < n && A[j + 1] == B[remE + 1] 
                        && find(temM.begin(), temM.end(), remE) == temM.end())
                        temM.push_back(remE + 1);
                    era_node.push_back(en);
                }

                if (!era_node.empty()) {
                    for (int en = era_node.size() - 1; en >= 0; en--) {
                        erase_array(EndPoint[j], EndPoint_num[j], era_node[en]); EndPoint_num[j]--;
                    }
                }
                
            }
            nexM = temM;
                
            int minCol = min(dk_ij[j - i + k], nexP);

            if (minCol > n) couK--;
            else couK = k;

            if (couK <= 0) {
                //cout << "i = " << i << ", j = " << j << endl;
                break;
            }

            dk_ij[j - i + k] = minCol;
            if (couK == k) maxL = max(maxL, j - i + k); 
        }
        
        if (m - i < maxL) break;
        //for (int i = 0; i <= maxL + 1; i++) cout << dk_ij[i] << ","; cout << endl;
    }
    // for (int i = 0; i <= maxL + 1; i++) cout << dk_ij[i] << ","; cout << endl;

    delete[] dk_ij;

    
    delete_2D_Array(EndPoint, m, n);
    delete_2D_Array(group, m + n + 1, n);

    delete[] groupA;
    delete[] groupB;

    delete[] group_num;
    delete[] EndPoint_num;

    return maxL;
}
