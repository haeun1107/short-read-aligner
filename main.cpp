
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include "suffix_array.cpp"
#include "spaced_seed_filter.cpp"
#include "smith_waterman.cpp"

using namespace std;

string readFile(const string& filename) {
    ifstream file(filename);
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    content.erase(remove(content.begin(), content.end(), '\n'), content.end());
    return content;
}

vector<string> readLines(const string& filename) {
    ifstream file(filename);
    vector<string> lines;
    string line;
    while (getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

int main() {
    auto start_time = chrono::high_resolution_clock::now();

    string target = readFile("reference.txt");
    vector<string> reads = readLines("reads.txt");

    ofstream out("result.txt");
    string pattern = "11111";
    int L = 50;

    for (int idx = 0; idx < reads.size(); ++idx) {
        string query = reads[idx];

        // 후보 제한
        vector<string> candidates;
        vector<int> positions;
        int window = 1000;
        int start = rand() % (target.length() - window - L);
        for (int i = start; i < start + window; ++i) {
            candidates.push_back(target.substr(i, L));
            positions.push_back(i);
        }

        vector<string> filtered = filterCandidates(query, candidates, pattern);

        AlignmentResult best;
        int bestScore = -1;
        int bestPos = -1;
        for (int i = 0; i < filtered.size(); ++i) {
            AlignmentResult res = smithWaterman(query, filtered[i]);
            if (res.score > bestScore) {
                bestScore = res.score;
                best = res;
                bestPos = positions[i];
            }
        }

        out << "Read[" << idx << "] " << query << endl;
        if (bestScore > 0) {
            out << "↳ Aligned Read : " << best.aligned_query << endl;
            out << "↳ Aligned Ref  : " << best.aligned_ref << " (at pos " << bestPos << ", score: " << best.score << ")\n\n";
        } else {
            out << "↳ No good match found.\n\n";
        }
    }

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end_time - start_time;
    cout << "⏱ 전체 실행 시간: " << elapsed.count() << "초" << endl;

    return 0;
}
