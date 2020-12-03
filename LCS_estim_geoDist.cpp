#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <chrono>
using namespace std;

string readFile(string path) {
    ifstream fin;
    fin.open(path);
    string s1;
    fin >> s1;
    fin.close();
    return s1;
}

int DP(string s1, string s2) {
    int s1_length = s1.size();
    int s2_length = s2.size();
    int* first = new int[s1_length + 1];
    for (int i = 0; i < s1_length + 1; i++) {
        first[i] = 0;
    }
    for (int j = 1; j < s2_length + 1; j++) {
        int* second = new int[s1_length + 1];
        second[0] = 0;
        for (int i = 1; i < s1_length + 1; i++) {
            if (s1[i - 1] == s2[j - 1])
                second[i] = max(first[i - 1] + 1, max(first[i], second[i - 1]));
            else
                second[i] = max(first[i], second[i - 1]);
        }
        //delete(first);
        first = second;
    }
    return  first[s1_length];
}

int LowBound(string s1, string s2) {
    return max(min(count(s1.begin(), s1.end(), 'A'), count(s2.begin(), s2.end(), 'A')), min(count(s1.begin(), s1.end(), 'T'), count(s2.begin(), s2.end(), 'T')));
}

int divide(string s1, string s2, int dNum) {
    int s1_length = s1.size();
    int s2_length = s2.size();

    int s1Sub = s1_length / dNum;
    int s2Sub = s2_length / dNum;

    int res = 0;
    for (int i = 0; i < dNum - 1; i++) res += LowBound(s1.substr(i * s1Sub, s1Sub), s2.substr(i * s2Sub, s2Sub));
    /*for (int i = 0; i < dNum - 1; i++) {
        int subResult = LowBound(s1.substr(i * s1Sub, s1Sub), s2.substr(i * s2Sub, s2Sub));
        cout << "i = " << i << ": sub-LCS = " << subResult << endl;
        res += subResult;
    }*/

    res += LowBound(s1.substr((dNum - 1) * s1Sub, s1_length - (dNum - 1) * s1Sub), s2.substr((dNum - 1) * s2Sub, s2_length - (dNum - 1) * s2Sub));
    return res;
}

int main() {
    string s1 = readFile("a1-75.in");
    //string s1 = readFile("a1-20.in");
    string s2 = readFile("a2-75.in");
    cout << "lenght of s1 =  " << s1.size() << ", length of s2 = " << s2.size() << endl;

    int DP_LCS = DP(s1, s2);
    cout << "DP 的結果是：" << DP_LCS << endl;

    //random_shuffle(s1.begin(), s1.end());
    //random_shuffle(s2.begin(), s2.end());

    int LB = LowBound(s1, s2);
    cout << "LowBound 的結果是：" << LB << endl;

    cout << "div = 1000, CS = " << divide(s1, s2, 1000) << endl;

    vector<int> divNum(20);
    vector<int> res(20);

    for (int i = 500; i <= 10000; i += 500) {
        divNum[i / 500 - 1] = i;
        res[i / 500 - 1] = divide(s1, s2, i);
    }

    cout << "divNum = ";
    for (int i = 0; i < 20; i++) cout << i << ", ";
    cout << endl;

    cout << "result = ";
    for (int i = 0; i < 20; i++) cout << res[i] << ", ";
    cout << endl;

}