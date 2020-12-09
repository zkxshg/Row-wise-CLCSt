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
#define MaxValue 9999999

using namespace std;

// ============================ 定义常量 ============================
//// 测试数据路径
//// 1000 * 1000; |bet| = 2;
static string fileName1 = "D:/Lab/Tex/7_Realdata/COVID-19/region/CN_MT019529.fasta"; 
static string fileName2 = "D:/Lab/Tex/7_Realdata/COVID-19/region/CN_BJ_EPI_ISL_469254.fasta";
static string constraintName = "D:/Lab/Tex/7_Realdata/COVID-19/frame/constraint.txt";
//static string fileName4 = "string_5000_5000_2_4.csv";

//// 5000 * 5000; |bet| = 8;
//static string fileName5 = "runtime_5000_5000_2_8.csv";// m_n_k_alphabet 
//static string fileName6 = "string_5000_5000_2_8.csv";

// 运行结果保存路径
static string savefile = "D:/Lab/Tex/8_save/data.csv";

// alphabet size
static int alphabet = 4;

// LCSk in k length substring
static int k = 5;

// length of string A
static int m = 500;

// length of string B
static int n = 500;

// length of constrainred string P
static int r = m / 20;

// ============================  Diagnol-CLCS by Hung ============================
// S.-H. Hung, C.-B. Yang, and K.-S. Huang, "A Diagonal-Based Algorithm for the Constrained Longest Common Subsequence Problem," 
// in International Computer Symposium, 2018, pp. 425-432: Springer.
//===== Diagnol-based: O(rL(m - L)) =====
void RandomSequence(int* A, int* B, int* P) {
    A[0] = 0, B[0] = 0, P[0] = 0;
    for (int i = 1; i <= m; i++)A[i] = rand() % alphabet + 1;
    for (int i = 1; i <= n; i++)B[i] = rand() % alphabet + 1;
    for (int i = 1; i <= r; i++)P[i] = rand() % alphabet + 1;
}

struct node {
    int i, j, k;
};
typedef struct node Node;

// create the success table of B
void construct_next(int* A, int* B, int* P, int** nextB) {
    int locationB;
    //nextB = new int* [alphabet + 1];
    //for (int i = 0; i < (alphabet + 1); i++) nextB[i] = new int[n + 1];

    for (int i = 1; i < (alphabet + 1); i++) {
        locationB = n + 1;
        for (int j = n; j > 0; j--) {
            if (j == n) nextB[i][j] = locationB;

            if (B[j] == i) {
                locationB = j;
                nextB[i][j - 1] = locationB;
            }
            else {
                nextB[i][j - 1] = locationB;
            }
        }
    }
}

// Diagnol-based CLCS
int hung_CLCS(int* Ao, int* Bo, int* Po) {
    int i, j, t, k, length, round, nowb, flag, max_len;

    int* A = new int[m + 1]; A[0] = 0; for (int i1 = 1; i1 <= m; i1++) A[i1] = Ao[i1 - 1];
    int* B = new int[m + 1]; B[0] = 0; for (int i1 = 1; i1 <= n; i1++) B[i1] = Bo[i1 - 1];
    int* C = new int[m + 1]; C[0] = 0; for (int i1 = 1; i1 <= r; i1++) C[i1] = Po[i1 - 1];

    int * limitB = new int[r + 1];

    Node point;
    point.i = MaxValue; point.j = MaxValue; point.k = MaxValue;

    vector<Node> v_column;
    v_column.assign(r + 1, point);

    vector<vector<Node> > v;
    v.assign(m + 1, v_column);

    for (i = 0; i < r + 1; i++) {
        v[0][i].i = 0; v[0][i].j = 0; v[0][i].k = 0;
    }

    /**initial limitB*/
    for (i = 0; i < r + 1; i++) limitB[i] = 0;

    /**find limitB*/
    i = r;
    for (j = n; j > 0; j--) {
        if (C[i] == B[j]) {
            limitB[i] = j;
            --i;
        }
    }

    /**no answer*/
    for (i = 1; i <= r; i++) {
        if (limitB[i] == 0) {
            delete[] limitB;
            return -MaxValue;
        }
    }

    /*cout << "limitB:";
    for(i = 1; i <= r; i++)//print limitB
        cout << limitB[i] << " ";
    cout << "\n";*/

    int** nextB = new int* [alphabet + 1];;
    for (int i = 0; i < (alphabet + 1); i++) nextB[i] = new int[n + 1];
    construct_next(A, B, C, nextB);//build success table

    max_len = 0;
    for (round = 1; round <= m; round++) {
        length = 0;

        //searching in A
        for (i = round; i <= m; i++) {
            // cout << "============== i = " << i << "======================= " << endl;
            //developing of all data
            for (j = 0; j < r + 1; j++) {

                if (v[length][j].k == MaxValue) continue;

                if (length == 0) k = 0;
                else k = r - v[length][j].k;
                //cout << "v[length][j].j : length = " << length << ", j = " << j << endl;
                nowb = v[length][j].j;
                //cout << "nextB[A[i]][nowb] : A[i] = " << A[i] << ", nowb = " << nowb << endl;
                if (nowb != MaxValue) t = nextB[A[i]][nowb]; //next match A[i] with B

                //constrained letters are found end if constrained letters are found
                if (k >= r) {
                    if (t <= n) {
                        if ((v[length + 1][0].j == MaxValue) && (v[length + 1][0].k == MaxValue)) {
                            v[length + 1][0].i = i; v[length + 1][0].j = t; v[length + 1][0].k = 0;
                        }
                        else if (t <= v[length + 1][0].j) {
                            v[length + 1][0].i = i; v[length + 1][0].j = t; v[length + 1][0].k = 0;
                        }
                    }
                }
                else {
                    //not exactly found

                    //A[i]=B[t]!=C[k+1]
                    if ((t <= limitB[k + 1]) && (A[i] != C[k + 1])) {
                        if ((v[length + 1][r - k].j == MaxValue) && (v[length + 1][r - k].k == MaxValue)) {
                            v[length + 1][r - k].i = i; v[length + 1][r - k].j = t; v[length + 1][r - k].k = r - k;
                        }
                        else if (t <= v[length + 1][r - k].j) {
                            v[length + 1][r - k].i = i; v[length + 1][r - k].j = t; v[length + 1][r - k].k = r - k;
                        }
                    }
                    else if ((t <= limitB[k + 1]) && (A[i] == C[k + 1])) {
                        //A[i]=B[t]=C[k+1]
                        k++;
                        if ((v[length + 1][r - k].j == MaxValue) && (v[length + 1][r - k].k == MaxValue)) {
                            v[length + 1][r - k].i = i; v[length + 1][r - k].j = t; v[length + 1][r - k].k = r - k;
                        }
                        else if (t <= v[length + 1][r - k].j) {
                            v[length + 1][r - k].i = i; v[length + 1][r - k].j = t; v[length + 1][r - k].k = r - k;
                        }
                    }
                }
                //end else not exactly found
            }
            //end for developing of all data

            t = 1;
            //domination
            for (k = 0; t < r + 1; )  {
                if (v[length + 1][k].j <= v[length + 1][t].j) {
                    v[length + 1][t].i = MaxValue; v[length + 1][t].j = MaxValue; v[length + 1][t].k = MaxValue;
                    t++;
                }
                else {
                    k = t; t++;
                }
            }
            //end domination

            flag = 0;
            for (j = 0; j < r + 1; j++) {
                if (v[length][j].i != MaxValue) flag = 1;
            }

            if (flag == 0) break;

            length++;
            if (length > max_len) max_len = length;

        }
        //end searching in A

        if (((m - round + 1) <= max_len) && (v[max_len - 1][0].k == 0)) break;
    }
    //end for

    for (j = 0; j < alphabet + 1; j++) {
        delete[] nextB[j];
    }
    delete[] nextB;
    delete[] limitB;

    for (i = max_len; i > 0; i--) {
        if (v[i][0].k == 0) return i;
    }

    return -MaxValue;
}
