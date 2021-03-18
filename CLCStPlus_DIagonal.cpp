#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <set>
#include <climits>
#include <unordered_map>
#include<map> 
#include <windows.h>
#pragma warning(disable:6385)
#pragma warning(disable:6386)
#define MaxValue 9999999

using namespace std;

// ============================ Diagnol-CLCSt+ ============================
 int Diagonal_CLCSt_plus(int* A, int* B, int* P, int m, int n, int r, int k) {
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

     // 2.5 记录每行match
     int** nexPoint;
     nexPoint = new int* [m];
     for (int i = 0; i < m; i++) nexPoint[i] = new int[n];

     int* nexPoint_num = new int[m];
     for (int i = 0; i < m; i++) nexPoint_num[i] = 0;

     for (int i = k - 1; i < m - 1; i++) {
         for (int z = 0; z < EndPoint_num[i]; z++) {
             int en = EndPoint[i][z] + 1;
             if (en < n && A[i + 1] == B[en]) {
                 nexPoint[i][nexPoint_num[i]] = en - 1;
                 nexPoint_num[i]++;
             }
         }
     }

     // 3 Diagonal LCSt+
     // Initialize
     int nk = n + 1;

     // vector initialize
     //vector<vector<int> > dt_ijk(nk, vector<int>(r + 1, n * 10));  // Diagoinal 数组
     //dt_ijk[0][0] = 0;

     // array initialize
     int** dt_ijk = new int* [nk];
     for (int h = 0; h < nk; h++) {
         dt_ijk[h] = new int[r + 1];
         for (int i = 0; i < r + 1; i++) dt_ijk[h][i] = n * 10;
     }
     dt_ijk[0][0] = 0;

     
     // Start
     int maxL = 0, unlimitL = 0;
     // Round i: O(m - L)
     double t1 = 0.0, t2 = 0.0, t3 = 0.0;
     for (int i = k - 1; i < m; i++) {

         int pos = 0, couK = k;
         vector<int> nexM;
         // set<int> nexM;

         // Row j: O(L)
         for (int j = i; j < m; j++) {

             int enLen = 0, nmLen = 0, nexE = n;

             // Constranit[c]: O(r + 1)
             for (int c = 0; c <= r; c++) {
                 //cout << "i = " << i << ", j = " << j << ", c = " << c << endl;
                 if (dt_ijk[j - i][c] > n && dt_ijk[j - i + k - 1][c] > n) break;

                 pos = dt_ijk[j - i][c];
                 int cpos = dt_ijk[j - i + k - 1][c]; // pos for the extension
                 //cout << "pos = " << pos << ", cpos = " << cpos << endl;

                 int nexP = n * 10, enRes = n * 10, nmRes = n * 10; 

                 // Search next Match: O(log(R/n))
                 if (pos <= n) {
                     int enm = BinarySearch_2(EndPoint[j], pos + k - 1, enLen, EndPoint_num[j] - 1);
                     if (enm >= 0) {
                         if (c == 0) nexE = EndPoint[j][enm];
                         enLen = enm;
                         enRes  = EndPoint[j][enm] + 1;
                         nexP = min(enRes, nexP);
                     }
                 }
                
                 // Search the nextM table: O(log(R/n))
                 if (cpos <= n && !nexM.empty()) {
                     int nms = nexM.size();
                     for (int nm = nmLen; nm < nms; nm++) {
                         if (nexM[nm] >= cpos) {
                             nmLen = nm; 
                             nmRes = nexM[nm] + 1; 
                             // cout << "nexM[nm]  = " << nexM[nm] << ", A[j] = " << A[j] << endl;
                             nexP = min(nmRes, nexP); break; 
                         }
                     }
                 }

                 // update the dij table
                 dt_ijk[j - i + k][c] = min(dt_ijk[j - i + k][c], nexP);
                 if (dt_ijk[j - i + k][c] > n) break;

                 // Update the constraints by t-match
                 if (c < r && enRes <= n) {
                     int cur_pre = maxPre(A, P, c, j);
                    // cout << "cur_pre = " << cur_pre << endl;
                     for (int pre = 1; pre <= cur_pre; pre++)
                         dt_ijk[j - i + k][c + pre] = min(dt_ijk[j - i + k][c + pre], enRes);
                 }

                 // Update the constraints by extension
                 if (c < r && A[j] == P[c] && nmRes <= n) dt_ijk[j - i + k][c + 1] = min(dt_ijk[j - i + k][c + 1], nmRes);

             }

             if (dt_ijk[j - i + k][r] <= n) maxL = max(maxL, j - i + k);
             if (dt_ijk[j - i + k][0] <= n) unlimitL = max(unlimitL, j - i + k);

             if (unlimitL + k < j - i) break;

             // Search the extensions: Total O(R)
             //set<int> temM;
             vector<int> temM;

             if (j + 1 < m) {
                 // search last nextM table
                 for (int nm : nexM)
                     if (nm + 1 < n && A[j + 1] == B[nm + 1])
                         temM.push_back(nm + 1);

                 // search rest Match pairs: Total O(R)
                 for (int en = 0; en < nexPoint_num[j]; en++) {
                     int remE = nexPoint[j][en];
                     if (remE >= nexE) temM.push_back(remE + 1);
                 }
                 
                 //sort(temM.begin(), temM.end()); nexM = temM;
                 set<int> st(temM.begin(), temM.end());
                 nexM.assign(st.begin(), st.end());

                 //nexM.resize(temM.size());
                 //int cou = 0;
                 //for (int tm : temM) {
                 //    nexM[cou] = tm; cou++;
                 //}

                 //// remove node s.t. col >= dij[maxL][maxC]
                 //vector<int> era_node;
                 //for (int en = enLen; en < EndPoint_num[j]; en++) era_node.push_back(en);
                 //if (!era_node.empty()) {
                 //    for (int en = era_node.size() - 1; en >= 0; en--) {
                 //        erase_array(EndPoint[j], EndPoint_num[j], era_node[en]); EndPoint_num[j]--;
                 //    }
                 //}

             }
             
         }
         
         //for (int tc = 0; tc <= r; tc++) {
         //    for (int ti = 0; ti <= unlimitL  +1; ti++)  cout << dt_ijk[ti][tc] << ","; cout << endl;
         //}
                  
         if (m - i < maxL) break;
         
     }
     // for (int i = 0; i <= maxL + 1; i++) cout << dk_ij[i] << ","; cout << endl;

     delete_2D_Array(EndPoint, m, n);
     delete_2D_Array(group, m + n + 1, n);
     delete_2D_Array(nexPoint, m, n);

     delete_2D_Array(dt_ijk, nk, r + 1);

     delete[] groupA;
     delete[] groupB;

     delete[] group_num;
     delete[] EndPoint_num;
     delete[] nexPoint_num;

     return maxL;
 }
