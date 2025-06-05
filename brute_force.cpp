#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm>

using namespace std;
using namespace chrono;

const int MAX_ERROR = 10;

// ✅ 파일 입출력
string loadReference(const string& file) {
    ifstream fin(file);
    string line, seq;
    while (getline(fin, line)) {
        if (line[0] == '>') continue;
        for (char c : line) {
            char uc = toupper(c);
            if (uc == 'A' || uc == 'T' || uc == 'G' || uc == 'C') seq += uc;
        }
    }
    return seq;
}

vector<string> loadReads(const string& file) {
    ifstream fin(file);
    vector<string> reads;
    string line;
    while (getline(fin, line)) {
        if (!line.empty()) reads.push_back(line);
    }
    return reads;
}

// ✅ mismatch 계산 함수
int countMismatch(const string& a, const string& b) {
    int count = 0;
    for (int i = 0; i < a.size(); ++i)
        if (a[i] != b[i]) count++;
    return count;
}

// ✅ 시간 측정 유틸리티
void printTime(const string& label, high_resolution_clock::time_point start, high_resolution_clock::time_point end) {
    double duration = chrono::duration_cast<chrono::duration<double>>(end - start).count();
    cout << label << " " << duration << "초\n";
}

// ✅ Brute-Force 정렬
int main() {
    auto total_start = high_resolution_clock::now();

    // 1. 입력 로딩
    auto t1 = high_resolution_clock::now();
    string reference = loadReference("reference.txt");
    vector<string> reads = loadReads("reads.txt");
    auto t2 = high_resolution_clock::now();
    printTime("[1] 입력 로딩:", t1, t2);

    // 2. Brute-force 정렬
    t1 = high_resolution_clock::now();
    ofstream fout("result_brute.txt");

    for (int i = 0; i < reads.size(); ++i) {
        const string& read = reads[i];
        int bestMismatch = MAX_ERROR + 1;
        int bestPos = -1;

        for (int j = 0; j + read.size() <= reference.size(); ++j) {
            string refSeg = reference.substr(j, read.size());
            int mismatch = countMismatch(read, refSeg);

            if (mismatch < bestMismatch) {
                bestMismatch = mismatch;
                bestPos = j;
                if (mismatch == 0) break;  // perfect match found
            }
        }

        fout << "read index: " << i << " -> ";
        if (bestMismatch <= MAX_ERROR)
            fout << "pos: " << bestPos << ", mismatch: " << bestMismatch << "\n";
        else
            fout << "no match\n";
    }

    fout.close();
    t2 = high_resolution_clock::now();
    printTime("[2] Brute-Force 정렬 완료:", t1, t2);

    auto total_end = high_resolution_clock::now();
    printTime("✅ 전체 수행 시간:", total_start, total_end);
    return 0;
}
