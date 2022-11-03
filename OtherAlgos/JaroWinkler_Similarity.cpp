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
#include <utility> 
#include <windows.h>

using namespace std;

static string fileName1 = "A.fasta";  
static string fileName2 = "B.fasta"; 

float p = 0.1f;
static float max_p = 0.25f;
static float max_l = 4;

pair<int, int> matchPairs(string& s1, string& s2) {
    int m = s1.size(), n = s2.size();

    vector<int> mpos(m); vector<int> npos(n); // Array to record if pos is matched

    int mw = max(max(m, n) / 2 - 1, 0); // length of matching window
    cout << "Length of matching window is: " << mw << endl;

    int mchNum = 0; // number of matches
    for (int i = 0; i < m; i++) {
        // Backward Search 
        for (int suf = mw; suf >= 0; suf--) {
            if (i - suf < 0) continue; // lower bound
            if (i - suf >= n) break; // upper bound
            if (npos[i - suf]) continue; // Visited

            if (s2[i - suf] == s1[i]) { // Find a match pair
                mpos[i] = 1; npos[i - suf] = 1;
                mchNum++;
            }
        }

        // Forward Search
        if (mpos[i]) continue;
        for (int pre = 1; pre <= mw; pre++) {
            if (i + pre >= n) break; // upper bound
            if (npos[i + pre]) continue; // Visited

            if (s2[i + pre] == s1[i]) { // Find a match pair
                mpos[i] = 1; npos[i + pre] = 1;
                mchNum++;
            }
        }
    }
    cout << "Number of match pairs is: " << mchNum << endl;

    // Calculate the transpositions
    int mt = 0, nt = 0, transNum = 0;
    while (mt < m && nt < n) {
        while (mt < m && !mpos[mt]) mt++;
        while (nt < n && !npos[nt]) nt++;

        if (mt >= m || nt >= n) break;

        if (s1[mt] != s2[nt]) transNum++;

        mt++; nt++;
    }
    cout << "Number of transpositions pairs is: " << transNum << endl;

    return {mchNum, transNum};
}

float JaroDist(string& s1, string& s2) {
    int m = s1.size(), n = s2.size();
    if (!m || !n) {cout << "Inputs are empty." << endl; return 0;}

    pair<int, int> mch_rev = matchPairs(s1, s2);
    float mchNum = mch_rev.first, transNum = mch_rev.second;
    //cout << "Number of matches is: " << mch_rev.first << endl;
    //cout << "Number of transpositions is: " << mch_rev.second << endl;

    if (mchNum <= 0) return 0;

    float JaroLen = (mchNum / m + mchNum / n + (mchNum - transNum / 2) / mchNum) / 3;

    return JaroLen;
}

float JaroWinklerDistance(string& s1, string& s2, float p) {
    int m = s1.size(), n = s2.size();
    if (!m || !n) { cout << "Inputs are empty." << endl; return 0; }

    p = min(p, max_p); // scaling factor 

    float jaroLen = JaroDist(s1, s2);

    // Calculate the length of common prefix
    int l = 0; // the length of common prefix up to 4
    for (int i = 0; i < min(m, n); i++) {
        if (s1[i] != s2[i]) break;
        l++; 
        if (l >= max_l) break;
    }

    cout << "jaroLen = " << jaroLen << ", l = " << l << ", p = " << p << endl;
    float JaroWinklerSim = jaroLen + l * p * (1 - jaroLen);

    return JaroWinklerSim;
}

int String_len(string fName) {
    ifstream infile;
    infile.open(fName);
    infile.seekg(0, ios_base::end);
    int nFileLen = infile.tellg();
    return nFileLen;
}

string String_Read(string fName) {

    int nFileLen = String_len(fName);
    cout << "length of " << fName << " is " << nFileLen << endl;

    string A;

    ifstream infile;
    infile.open(fName);
    infile >> A;
    infile.close();

    return A;
}

int main(int argc, char** argv) {
    string A = String_Read(fileName1);
    //string A = "abcdefgh";

    string B = String_Read(fileName2);
    // string B = "abehc";

    float jaroLen = JaroDist(A, B);
    cout << "JaroDist is: " << jaroLen << endl;

    float JWDist = JaroWinklerDistance(A, B, 0.1);
    cout << "JaroWinklerDistance is: " << JWDist << endl;

    return 0;
}