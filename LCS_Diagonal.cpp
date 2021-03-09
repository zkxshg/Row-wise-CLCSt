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
#define MaxValue 9999999

using namespace std;

// ====== 创建 Successor Table: O(alpha * n) ======
vector<vector<int> > succTable(int* A, int m, int alpha) {
    int len = m;
    vector<vector<int> > succesT(alpha + 1, vector<int>(len + 1, len * 10));
  
    for (int i = len - 1; i >= 0; i--) {
        for (int j = 1; j <= alpha; j++) {
            if (A[i] == j) succesT[j][i] = i + 1; 
            else succesT[j][i] = succesT[j][i + 1];
        }
    }
  
    return succesT;
}

// ============================ DIagonal LCS ============================
// N. Nakatsu, "A longest common subsequence algorithm suitable for similar text strings,", 1982.
// ===== Diagonal O(n * (m - L)) =====
int Diagonal_LCS(int* A, int* B, int m, int n) {
    
    // 0 Initialize
    int* d_ij = new int[n]; 
    for (int i = 0; i < n; i++) d_ij[i] = n * 10;

    // 1 calcuulate successor table
    vector<vector<int> > stB = succTable(B, n, alphabet);

    // 2 Diagonal LCS
    int maxL = 0;
    for (int i = 0; i < m; i++) {
        int pos = 0;
        for (int j = i; j < m; j++) {
            int nexP = stB[A[j]][pos];

            int minCol = min(d_ij[j - i], nexP);
            if (minCol > n) break;

            d_ij[j - i] = minCol;
            maxL = max(maxL, j - i + 1);
            pos = d_ij[j - i];
        }
        if (m - i < maxL) break;
    }

    // 3 get result
    //int res = 0;
    //for (int i = n - 1; i >= 0; i--) {
    //    if (d_ij[i] <= n) {
    //        res = i; break;
    //    }
    //}

    delete[] d_ij;
    return maxL;
}
