
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

vector<int> buildSuffixArray(const string &s) {
    int n = s.length();
    vector<int> sa(n), ranks(n), tmp(n);

    for (int i = 0; i < n; ++i) {
        sa[i] = i;
        ranks[i] = s[i];
    }

    for (int k = 1;; k <<= 1) {
        auto compare = [&](int i, int j) {
            if (ranks[i] != ranks[j]) return ranks[i] < ranks[j];
            int ri = i + k < n ? ranks[i + k] : -1;
            int rj = j + k < n ? ranks[j + k] : -1;
            return ri < rj;
        };
        sort(sa.begin(), sa.end(), compare);

        tmp[sa[0]] = 0;
        for (int i = 1; i < n; ++i)
            tmp[sa[i]] = tmp[sa[i - 1]] + (compare(sa[i - 1], sa[i]) ? 1 : 0);
        ranks = tmp;
        if (ranks[sa[n - 1]] == n - 1) break;
    }

    return sa;
}
