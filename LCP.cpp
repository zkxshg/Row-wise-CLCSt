// Longest Common Prefix
  
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

// 后缀结构
struct suffix {
    int index;  // To store original index 
    int rank[2]; // To store ranks and next rank pair 
};

// 基于字典序比较 suffix 大小：1: a < b; 0: a > b 
int cmp(struct suffix a, struct suffix b) {
    return (a.rank[0] == b.rank[0]) ? (a.rank[1] < b.rank[1] ? 1 : 0) :
        (a.rank[0] < b.rank[0] ? 1 : 0);
}

// 创建后缀数组
vector<int> buildSuffixArray(int* A, int n) {
    // ========== 0. Initialize the suffix array ==========
    
    // A structure to store suffixes and their indexes
    struct suffix * suffixes = new suffix[n];

    // Store suffixes and their indexes in an array of structures. 
    // The structure is needed to sort the suffixes alphabatically and maintain their old indexes while sorting. 
    for (int i = 0; i < n; i++) {
        suffixes[i].index = i;
        suffixes[i].rank[0] = A[i];
        suffixes[i].rank[1] = ((i + 1) < n) ? (A[i + 1]) : -1;
    }

    // ========== 1. Sort the suffixes using the comparison function defined above.  ==========
    sort(suffixes, suffixes + n, cmp);

    // At his point, all suffixes are sorted according to first 2 characters.  
    // Let us sort suffixes according to first 4 characters, then first 8 and so on. 
    
    // ========== 3. Initial the suffix -> index mapping ==========
    // This array is needed to get the index in suffixes[] from original index.
    // This mapping is needed to get next suffix.
    int * ind = new int[n];  

    // ========== 4. Radix sort of suffixes ==========
    for (int k = 4; k < 2 * n; k = k * 2) {
        // 4.1 Assigning rank and index values to first suffix 
        int rank = 0;
        int prev_rank = suffixes[0].rank[0];
        suffixes[0].rank[0] = rank;   
        ind[suffixes[0].index] = 0;

        // 4.2 Assigning rank to suffixes 
        for (int i = 1; i < n; i++) {
            // If first rank and next ranks are same as that of previous suffix in array, assign the same new rank to this suffix 
            if (suffixes[i].rank[0] == prev_rank &&
                suffixes[i].rank[1] == suffixes[i - 1].rank[1]) {
                prev_rank = suffixes[i].rank[0];
                suffixes[i].rank[0] = rank;
            } else {
                // Otherwise increment rank and assign 
                prev_rank = suffixes[i].rank[0];
                suffixes[i].rank[0] = ++rank;
            }
            ind[suffixes[i].index] = i;
        }

        // 4.3 Assign next rank to every suffix 
        for (int i = 0; i < n; i++) {
            int nextindex = suffixes[i].index + k / 2;
            suffixes[i].rank[1] = (nextindex < n) ?
                suffixes[ind[nextindex]].rank[0] : -1;
        }

        // 4.4 Sort the suffixes according to first k characters 
        sort(suffixes, suffixes + n, cmp);
    }

    // ========== 5. Store indexes of all sorted suffixes in the suffix array ========== 
    vector<int> suffixArr;
    for (int i = 0; i < n; i++) suffixArr.push_back(suffixes[i].index);

    delete[] ind;
    delete[] suffixes;
    // Return the suffix array 
    return  suffixArr;
}

/ calculate the LCP
vector<int> kasai(int* A, vector<int> suffixArr) {
    int n = suffixArr.size();

    // ========== 0. To store LCP array ==========
    vector<int> lcp(n, 0);

    // ========== 1. An auxiliary array to store inverse of suffix array elements.  ==========
    // For example if suffixArr[0] is 5, the invSuff[5] would store 0.  
    // This is used to get next suffix string from suffix array. 
    vector<int> invSuff(n, 0);

    // Fill values in invSuff[] 
    for (int i = 0; i < n; i++) invSuff[suffixArr[i]] = i;

    // Initialize length of previous LCP 
    int k = 0;

    // ========== 3.Process all suffixes one by one starting from first suffix in txt[] ==========
     
    // 3.1 boundary condition
    // If the current suffix is at n-1, then we don’t have next substring to consider. 
    // So lcp is not defined for this substring, we put it zero.
    for (int i = 0; i < n; i++) {
        if (invSuff[i] == n - 1) {
            k = 0;
            continue;
        }

        // 3.2 j contains index of the next substring to be considered to compare with the present substring, 
        // i.e., next string in suffix array.
        int j = suffixArr[invSuff[i] + 1];

        // 3.3 Directly start matching from k'th index as at-least k-1 characters will match 
        while (i + k < n && j + k < n && A[i + k] == A[j + k]) k++;

        lcp[invSuff[i]] = k; // lcp for the present suffix. 

        // 3.4 Deleting the starting character from the string. 
        if (k > 0) k--;
    }

    // return the constructed lcp array 
    return lcp;
}
