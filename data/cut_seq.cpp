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
// #include <windows.h>

using namespace std;

static string fileName = "/Expre Design/Exp1_diff_virus/Ref_COV19_EPI_ISL_406801.fasta";
static string savefile = "/Expre Design/Exp1_diff_virus/A.fasta";

// ==================== 主测试函数 ====================
int main(int argc, char** argv) {
    // int argc, char** argv
    //// ========== = 读入参数 ========== =

    fileName = argv[1];
    savefile = argv[2];
    int st = atoi(argv[3]), en = atoi(argv[4]);

    ifstream fin(fileName);
    string wholeS = "";

    cout << "read file: " << fileName << endl;

    string s;
    while (fin >> s) wholeS += s;

    cout << "cut subsequnce" << endl;
    string sub = wholeS.substr(st, en - st);

    cout << "The segment is: " << sub << endl;

    ofstream outFile;
    outFile.open(savefile, ios::out); 
    outFile << sub;; outFile << endl;

    outFile.close();

    system("pause");
    return 0;
}
