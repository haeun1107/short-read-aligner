
#include <vector>
#include <string>

using namespace std;

vector<string> filterCandidates(const string &query, const vector<string> &candidates, const string &pattern) {
    vector<string> filtered;
    for (const string &cand : candidates) {
        bool match = true;
        for (int i = 0; i < pattern.size(); ++i) {
            if (pattern[i] == '1' && query[i] != cand[i]) {
                match = false;
                break;
            }
        }
        if (match) filtered.push_back(cand);
    }
    return filtered;
}
