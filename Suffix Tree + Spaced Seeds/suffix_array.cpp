#include <vector>
#include <string>
#include <algorithm>
#include <numeric>

using namespace std;

// 접미사 배열(Suffix Array) 구축 함수
// 입력 문자열의 모든 접미사를 정렬한 후, 해당 접미사의 시작 인덱스를 배열로 반환
vector<int> buildSuffixArray(const string& s_input) {
    string s = s_input + '\0';  // 널 문자를 끝에 추가 (정렬 안정성 확보 목적)
    int n = s.size();

    // ACGT 문자열을 숫자로 변환하여 처리
    vector<int> s_int(n);
    for (int i = 0; i < n; ++i) {
        if (s[i] == 'A') s_int[i] = 1;
        else if (s[i] == 'C') s_int[i] = 2;
        else if (s[i] == 'G') s_int[i] = 3;
        else if (s[i] == 'T') s_int[i] = 4;
        else s_int[i] = 0;  // 널 문자나 기타 문자
    }

    // sa: 접미사 배열, rank: 현재 접미사의 랭킹, tmp: 새 랭킹 저장용
    vector<int> sa(n), rank = s_int, tmp(n);
    iota(sa.begin(), sa.end(), 0);  // sa를 0~n-1로 초기화

    // Doubling 알고리즘으로 접미사 정렬 수행
    for (int k = 1;; k <<= 1) {
        // 랭킹 기반 비교 함수 정의 (k간격 비교)
        auto cmp = [&](int i, int j) {
            if (rank[i] != rank[j]) return rank[i] < rank[j];
            int ri = (i + k < n) ? rank[i + k] : -1;
            int rj = (j + k < n) ? rank[j + k] : -1;
            return ri < rj;
        };

        sort(sa.begin(), sa.end(), cmp);  // 접미사 정렬

        // 새로운 랭크 생성
        tmp[sa[0]] = 0;
        for (int i = 1; i < n; ++i) {
            tmp[sa[i]] = tmp[sa[i - 1]] + (cmp(sa[i - 1], sa[i]) ? 1 : 0);
        }

        rank = tmp;  // 랭크 갱신
        if (rank[sa[n - 1]] == n - 1) break;  // 모든 랭크가 고유하면 종료
    }

    sa.erase(sa.begin());  // 마지막에 추가한 널 문자 인덱스 제거
    return sa;
}

// LCP(Longest Common Prefix) 배열 생성 함수
// sa[i]와 sa[i+1]의 공통 접두사 길이를 계산하여 저장
vector<int> buildLCPArray(const string& s, const vector<int>& sa) {
    int n = s.size();
    vector<int> rank(n), lcp(n - 1);

    // sa 배열을 기반으로 각 인덱스의 랭크 정보 구성
    for (int i = 0; i < n; ++i)
        rank[sa[i]] = i;

    int h = 0;  // 공통 접두사 길이 누적값
    for (int i = 0; i < n; ++i) {
        if (rank[i] == 0) continue;  // 첫 번째는 비교할 이전이 없음
        int j = sa[rank[i] - 1];     // 이전 접미사의 시작 위치

        // i와 j로부터 시작되는 문자열의 공통 접두사 길이 계산
        while (i + h < n && j + h < n && s[i + h] == s[j + h])
            ++h;

        lcp[rank[i] - 1] = h;  // 해당 LCP 저장

        if (h > 0) --h;  // 다음 비교를 위해 h 감소 (효율적 LCP 계산)
    }

    return lcp;
}

// 이진 탐색 기반 접두사 매칭 범위 탐색 함수
// pattern과 접두사가 일치하는 부분을 접미사 배열 내에서 찾음
pair<int, int> findPrefixRange(const string& ref, const vector<int>& sa, const vector<int>& lcp, const string& pattern) {
    int n = sa.size(), m = pattern.size();
    int low = 0, high = n - 1;

    // 좌측 경계 찾기 (pattern 이상 처음 위치)
    while (low < high) {
        int mid = (low + high) / 2;
        if (ref.compare(sa[mid], m, pattern) < 0)
            low = mid + 1;
        else
            high = mid;
    }
    int left = low;

    // 우측 경계 찾기 (pattern 초과 위치)
    high = n;
    while (low < high) {
        int mid = (low + high) / 2;
        if (ref.compare(sa[mid], m, pattern) <= 0)
            low = mid + 1;
        else
            high = mid;
    }
    int right = low;

    // [left, right) 구간이 pattern으로 시작하는 접미사 위치들
    return {left, right};
}