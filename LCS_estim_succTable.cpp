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
#include <unordered_map>
#include <utility>
using namespace std;

string readFile(string path) {
	ifstream fin;
	fin.open(path);
	// cout << fin.is_open() << endl;
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

unordered_map<char, int> alphasetSize(string A, string B) {
	unordered_map<char, int> charCou;
	for (char ch : A) charCou[ch]++;
	for (char ch : B) charCou[ch]++;
	return charCou;
}

vector<vector<int> > succTable(string A, unordered_map<char, int> charMap, int alpha) {
	int len = A.size();
	vector<vector<int> > succesT(alpha, vector<int>(len + 1, len * 10));
	for (int i = len - 1; i >= 0; i--) {
		for (int j = 0; j < alpha; j++) {
			if (charMap[A[i]] == j) succesT[j][i] = i + 1;
			else succesT[j][i] = succesT[j][i + 1];
		}
	}
	return succesT;
}

int matchScan(vector<vector<int>>& ST1, vector<vector<int>>& ST2, unordered_map<char, int>& charMap, unordered_map<char, int> alphaDict) {
	int s1_length = ST1[0].size();
	int s2_length = ST2[0].size();
	int alpha = charMap.size();

	unordered_map<int, char> invMap;
	for (auto& v : charMap) invMap[v.second] = v.first;

	int pos1 = 0, pos2 = 0;
	vector<char> result;
	
	int freSum = 0;
	for (auto& v : alphaDict) freSum += v.second;

	int UB = 10 * (pow(s1_length, 2) + pow(s2_length, 2));
	while (pos1 < s1_length && pos2 < s2_length) {
		vector<double> dist(alpha, INT_MAX);

		for (auto& v : alphaDict) {
			int vindex = charMap[v.first];
			int posA = ST1[vindex][pos1];
			int posB = ST2[vindex][pos2];
			// dist[vindex] = pow(posA - pos1, 2) + pow(posB - pos2, 2) + double(v.second) / freSum;
			dist[vindex] = pow(posA, 2) + pow(posB, 2) + double(v.second) / freSum;
		}

		int minP = min_element(dist.begin(), dist.end()) - dist.begin();
		char minCh = invMap[minP];

		int newp1 = ST1[charMap[minCh]][pos1];
		int newp2 = ST1[charMap[minCh]][pos2];

		freSum -= (newp1 - pos1);
		freSum -= (newp2 - pos2);
		for (int i = pos1; i < newp1; i++) alphaDict[invMap[i]]--;
		for (int i = pos2; i < newp2; i++) alphaDict[invMap[i]]--;

		result.push_back(minCh);
		pos1 = newp1;
		pos2 = newp2;

		//cout << "char = " << invMap[minP] << ", pos1 = " << pos1 << ", pos2 = " << pos2 << ",dist = " << dist[minP] << endl;
	}

	//cout << "LCS 结果是：" << endl;
	//for (char ch : result) cout << ch << ",";
	//cout << endl;

	return result.size();
}

int main() {
	string s1 = readFile("LCS-Competition_1201/a1.in");
	string s2 = readFile("LCS-Competition_1201/a2.in");
	cout << "lenght of s1 =  " << s1.size() << ", length of s2 = " << s2.size() << endl;

	int DP_LCS = DP(s1, s2);
	cout << "DP 的結果是：" << DP_LCS << endl;

	unordered_map<char, int> alphaDict = alphasetSize(s1, s2);

	vector<pair<char, int> >  alphaSet;
	for (auto& v : alphaDict) alphaSet.push_back(make_pair(v.first, v.second));
	//cout << "alphaSet 的結果是：" << endl;
	//for (pair<char, int>& v : alphaSet) cout << "char =  " << v.first << ", fre = " << v.second << endl;

	unordered_map<char, int> charMap;
	int index = 0;
	for (pair<char, int>& v : alphaSet) charMap[v.first] = index++;
	//cout << "charMap 的結果是：" << endl;
	//for (auto& v : charMap) cout << "char =  " << v.first << ", fre = " << v.second << endl;

	vector<vector<int> > sT_A = succTable(s1, charMap, alphaSet.size());
	vector<vector<int> > sT_B = succTable(s2, charMap, alphaSet.size());
	/*cout << "sT_A 的結果是：" << endl;
	for (vector<int>& v : sT_A) {
		for (int i = 0; i < 30; i++) cout << s1[i] << ",  ";
		cout << endl;
		for (int i = 0; i < 30; i++) cout << v[i] << ",  ";
		cout << endl;
	}
	cout << "sT_B 的結果是：" << endl;
	for (vector<int>& v : sT_B) {
		for (int i = 0; i < 30; i++) cout << s2[i] << ",  ";
		cout << endl;
		for (int i = 0; i <30; i++) cout << v[i] << ",  ";
		cout << endl;
	}*/

	int res = matchScan(sT_A, sT_B, charMap, alphaDict);
	cout << "match scan 的结果是：" << res << endl;

	return 0;
}