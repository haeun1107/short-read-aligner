#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <numeric>

using namespace std;
using namespace chrono;

// 최대 허용 mismatch 수 (즉, 편집 거리 허용치)
const int MAX_ERROR = 10;

// 시드 길이 (접두사 기반 검색 범위 지정용)
const int SEED_LEN = 10;

// spaced seed 패턴: 1이면 해당 위치를 비교, 0이면 무시
const int SPACED_SEED_PATTERN[] = {1, 1, 0, 1, 1, 0, 1};

// 시간 측정용 유틸 함수
void printTime(const string& label, high_resolution_clock::time_point start, high_resolution_clock::time_point end) {
    double duration = chrono::duration_cast<chrono::duration<double>>(end - start).count();
    cout << label << " " << duration << "초\n";
}

// 접미사 배열(Suffix Array) 구축 함수 (Doubling 방식)
vector<int> buildSuffixArray(const string& s_input) {
    string s = s_input + '\0'; // 문자열 마지막에 널 문자 추가
    int n = s.size();

    // 문자 → 숫자 치환
    vector<int> s_int(n);
    for (int i = 0; i < n; ++i) {
        if (s[i] == 'A') s_int[i] = 1;
        else if (s[i] == 'C') s_int[i] = 2;
        else if (s[i] == 'G') s_int[i] = 3;
        else if (s[i] == 'T') s_int[i] = 4;
        else s_int[i] = 0;
    }

    vector<int> sa(n), rank = s_int, tmp(n);
    iota(sa.begin(), sa.end(), 0); // sa 초기화 (0 ~ n-1)

    for (int k = 1;; k <<= 1) {
        // rank 기준 정렬 비교 함수 정의
        auto cmp = [&](int i, int j) {
            if (rank[i] != rank[j]) return rank[i] < rank[j];
            int ri = (i + k < n) ? rank[i + k] : -1;
            int rj = (j + k < n) ? rank[j + k] : -1;
            return ri < rj;
        };
        sort(sa.begin(), sa.end(), cmp); // 접미사 정렬 수행

        // 새 랭크 할당
        tmp[sa[0]] = 0;
        for (int i = 1; i < n; ++i)
            tmp[sa[i]] = tmp[sa[i - 1]] + (cmp(sa[i - 1], sa[i]) ? 1 : 0);
        rank = tmp;
        if (rank[sa[n - 1]] == n - 1) break; // 모든 순위가 고유하면 종료
    }

    sa.erase(sa.begin()); // 마지막 null 제거
    return sa;
}

// LCP(Longest Common Prefix) 배열 구축 함수 (Kasai 알고리즘)
vector<int> buildLCPArray(const string& s, const vector<int>& sa) {
    int n = s.size();
    vector<int> rank(n), lcp(n - 1);
    for (int i = 0; i < n; ++i) rank[sa[i]] = i;

    int h = 0;
    for (int i = 0; i < n; ++i) {
        if (rank[i] == 0) continue;
        int j = sa[rank[i] - 1];
        while (i + h < n && j + h < n && s[i + h] == s[j + h]) ++h;
        lcp[rank[i] - 1] = h;
        if (h > 0) --h;
    }
    return lcp;
}

// spaced seed 매칭 (패턴 위치 기반 간략 비교)
bool spacedSeedMatch(const string& ref, const string& read, int pos) {
    int match = 0, total = 0;
    for (int i = 0; i < min(read.size(), sizeof(SPACED_SEED_PATTERN)/sizeof(int)); i++) {
        if (SPACED_SEED_PATTERN[i]) {
            if (ref[pos + i] == read[i]) match++;
            total++;
        }
    }
    return match >= total - (MAX_ERROR / 2); // 일정 비율 이상 일치 시 통과
}

// mismatch 수 계산
int countMismatch(const string& a, const string& b) {
    int count = 0;
    for (int i = 0; i < a.size(); ++i)
        if (a[i] != b[i]) count++;
    return count;
}

// 이진 탐색을 통한 접두사 검색 범위 찾기
pair<int, int> findPrefixRange(const string& ref, const vector<int>& sa, const vector<int>& lcp, const string& pattern) {
    int n = sa.size(), m = pattern.size();
    int low = 0, high = n - 1;

    // 왼쪽 경계 찾기
    while (low < high) {
        int mid = (low + high) / 2;
        if (ref.compare(sa[mid], m, pattern) < 0)
            low = mid + 1;
        else
            high = mid;
    }
    int left = low;

    // 오른쪽 경계 찾기
    high = n;
    while (low < high) {
        int mid = (low + high) / 2;
        if (ref.compare(sa[mid], m, pattern) <= 0)
            low = mid + 1;
        else
            high = mid;
    }
    int right = low;

    return {left, right};
}

// FASTA 형식 reference 파일 로드
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

// read 목록 로드 (1줄 1 read)
vector<string> loadReads(const string& file) {
    ifstream fin(file);
    vector<string> reads;
    string line;
    while (getline(fin, line)) {
        if (!line.empty()) reads.push_back(line);
    }
    return reads;
}

// 메인 함수: 전체 파이프라인 실행
int main() {
    auto total_start = high_resolution_clock::now();

    auto t1 = high_resolution_clock::now();
    string reference = loadReference("reference.txt");
    vector<string> reads = loadReads("reads.txt");
    auto t2 = high_resolution_clock::now();

    cout << "Reference length (N): " << reference.size() << " bp\n";
    cout << "Number of reads (M): " << reads.size() << " reads\n";
    printTime("[1] 입력 로딩:", t1, t2);

    t1 = high_resolution_clock::now();
    vector<int> sa = buildSuffixArray(reference);
    vector<int> lcp = buildLCPArray(reference + '\0', sa);
    t2 = high_resolution_clock::now();
    printTime("[2] SA + LCP 생성:", t1, t2);

    t1 = high_resolution_clock::now();
    ofstream fout("result.txt");

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < reads.size(); ++i) {
        const string& read = reads[i];
        auto [l, r] = findPrefixRange(reference, sa, lcp, read.substr(0, SEED_LEN));

        int bestMismatch = MAX_ERROR + 1, bestPos = -1;

        for (int j = l; j < r; ++j) {
            int pos = sa[j];
            if (pos + read.size() > reference.size()) continue;
            if (!spacedSeedMatch(reference, read, pos)) continue;

            string refseg = reference.substr(pos, read.size());
            int mismatch = countMismatch(read, refseg);

            if (mismatch < bestMismatch) {
                bestMismatch = mismatch;
                bestPos = pos;
                if (mismatch == 0) break;
            }
        }

        #pragma omp critical
        {
            fout << "read index: " << i << " -> ";
            if (bestPos != -1 && bestMismatch <= MAX_ERROR)
                fout << "pos: " << bestPos << ", mismatch: " << bestMismatch << "\n";
            else
                fout << "no match\n";
        }
    }

    fout.close();
    t2 = high_resolution_clock::now();
    printTime("[3] 정렬 수행 완료:", t1, t2);

    auto total_end = high_resolution_clock::now();
    
    printTime("✅ 전체 수행 시간:", total_start, total_end);
    return 0;
}
