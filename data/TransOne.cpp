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

static string fileName = "D:/Lab/Tex-�����޸�/7_Realdata/Expre Design/Exp1_diff_virus/Ref_COV19_EPI_ISL_406801.fasta";
static string savefile = "D:/Lab/Tex-�����޸�/7_Realdata/Expre Design/Exp1_diff_virus/A.fasta";

// ==================== �����Ժ��� ====================
int main(int argc, char** argv) {
    // int argc, char** argv
    //// ========== = ������� ========== =

    fileName = argv[1];
    savefile = argv[2];

    ifstream fin(fileName);
    string wholeS = "";

    cout << "read file: " << fileName << endl;

    string s;
    while (fin >> s) wholeS += s;

    ofstream outFile;
    outFile.open(savefile, ios::out); 
    outFile << wholeS; outFile << endl;

    outFile.close();

    system("pause");
    return 0;
}
