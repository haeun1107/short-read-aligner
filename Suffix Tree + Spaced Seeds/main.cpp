#include <iostream>     // 표준 입출력 스트림 사용
#include <vector>       // 벡터 자료구조 사용
#include <string>       // 문자열 처리
#include <fstream>      // 파일 입출력 스트림 사용
#include <chrono>       // 시간 측정을 위한 라이브러리
#include "suffix_array.cpp"   // 접미사 배열(SA) 및 LCP 생성 관련 함수 포함
#include "utils.cpp"          // 입력 로딩, mismatch 계산 등 유틸 함수
#include "spaced_seed.cpp"    // spaced seed 기반 필터링 함수

using namespace std;
using namespace chrono;

int main() {
    // 전체 수행 시간 측정 시작
    auto total_start = high_resolution_clock::now();

    // [1] 입력 로딩
    auto t1 = high_resolution_clock::now();
    string reference = loadReference("reference.txt");     // 참조 서열 로딩
    vector<string> reads = loadReads("reads.txt");         // 읽기(reads) 서열 로딩
    auto t2 = high_resolution_clock::now();
    printTime("[1] 입력 로딩:", t1, t2);                   // 경과 시간 출력

    // [2] 접미사 배열(Suffix Array) 및 LCP 생성
    t1 = high_resolution_clock::now();
    vector<int> sa = buildSuffixArray(reference);          // 접미사 배열 생성
    vector<int> lcp = buildLCPArray(reference + '\0', sa); // LCP 배열 생성 (문자열 종료문자 필요)
    t2 = high_resolution_clock::now();
    printTime("[2] SA + LCP 생성:", t1, t2);               // 경과 시간 출력

    // [3] 매칭 수행 및 결과 기록
    t1 = high_resolution_clock::now();
    ofstream fout("result.txt");                           // 결과 저장용 출력 파일

    // OpenMP 병렬 for 루프: 각 read에 대해 병렬 처리
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < reads.size(); ++i) {
        const string& read = reads[i];

        // [3-1] 접두사 검색: read의 앞부분(SEED_LEN) 기준으로 SA에서 후보 위치 탐색
        auto [l, r] = findPrefixRange(reference, sa, lcp, read.substr(0, SEED_LEN));

        int bestMismatch = MAX_ERROR + 1; // 가장 적은 mismatch 수
        int bestPos = -1;                // 가장 잘 맞는 위치

        // [3-2] 후보 영역 내 탐색
        for (int j = l; j < r; ++j) {
            int pos = sa[j];

            // 경계 검사: reference 범위를 벗어나면 건너뜀
            if (pos + read.size() > reference.size()) continue;

            // spaced seed 검사로 빠르게 필터링 (rough한 filter)
            if (!spacedSeedMatch(reference, read, pos)) continue;

            // 후보 구간 추출 후 mismatch 계산
            string refseg = reference.substr(pos, read.size());
            int mismatch = countMismatch(read, refseg);

            // 최소 mismatch 업데이트
            if (mismatch < bestMismatch) {
                bestMismatch = mismatch;
                bestPos = pos;
                if (mismatch == 0) break; // 완전 일치 시 조기 종료
            }
        }

        // [3-3] 출력 (병렬 충돌 방지 위해 critical 영역에서 출력)
        #pragma omp critical
        {
            fout << "read index: " << i << " -> ";
            if (bestPos != -1 && bestMismatch <= MAX_ERROR)
                fout << "pos: " << bestPos << ", mismatch: " << bestMismatch << "\n";
            else
                fout << "no match\n";
        }
    }

    fout.close();                      // 결과 파일 닫기
    t2 = high_resolution_clock::now();
    printTime("[3] 정렬 수행 완료:", t1, t2);   // 경과 시간 출력

    // 전체 종료 시간 측정
    auto total_end = high_resolution_clock::now();
    printTime("✅ 전체 수행 시간:", total_start, total_end);

    return 0;
}