#include <string>

// Spaced Seed Pattern 정의
// 1: 비교할 위치, 0: 비교하지 않을 위치 (즉, gap)
// 이 예시에서는 7자리 중 5자리를 비교함 (1 1 0 1 1 0 1)
const int SPACED_SEED_PATTERN[] = {1, 1, 0, 1, 1, 0, 1};

// spacedSeedMatch 함수
// 입력된 reference genome(ref)의 특정 위치(pos)부터 read와 비교하면서
// spaced seed 패턴에 따라 일치율을 계산하고, 일정 기준 이상이면 true 반환
bool spacedSeedMatch(const std::string& ref, const std::string& read, int pos) {
    int match = 0; // spaced seed 상 비교 위치 중 일치한 횟수
    int total = 0; // 비교한 총 위치 수 (패턴에서 1인 자리 수)

    // read와 spaced seed pattern을 기반으로 비교 수행
    for (int i = 0; i < std::min(read.size(), sizeof(SPACED_SEED_PATTERN)/sizeof(int)); i++) {
        if (SPACED_SEED_PATTERN[i]) {  // 비교가 필요한 위치일 때만 수행
            if (ref[pos + i] == read[i]) match++;  // 일치 시 match 증가
            total++;  // 비교 횟수 증가
        }
    }

    // 일정 수준 이상 일치했는지를 판단 기준으로 사용
    // ex) MAX_ERROR이 20이라면 절반인 10만큼 허용하므로 total - 10 이상 일치해야 true
    return match >= total - (MAX_ERROR / 2);
}