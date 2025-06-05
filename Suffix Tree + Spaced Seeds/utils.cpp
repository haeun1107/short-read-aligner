#include <iostream> 
#include <fstream> 
#include <vector>
#include <string>  
#include <chrono>

using namespace std;
using namespace chrono; // 시간 관련 객체 생략 없이 사용 가능하게 함

// 최대 허용 mismatch 개수 (예: alignment할 때 최대 허용되는 틀린 문자 수)
const int MAX_ERROR = 20;

// spaced seed 등의 prefix 기반 탐색 시 사용하는 시드 길이 (초기 비교 문자열 길이)
const int SEED_LEN = 20;

/**
 * @brief FASTA 형식의 참조 유전체 파일을 로드하는 함수
 * 
 * @param file 파일 경로 (Homo_sapiens.GRCh38.dna.alt.fa)
 * @return 대문자로 정제된 A/T/G/C 문자열로만 이루어진 참조 유전체 서열
 * 
 * 주의:
 * - FASTA 파일은 첫 줄이 '>'로 시작하는 주석 라인이며, 이후 줄이 실제 서열이다.
 * - 이 함수는 주석 줄을 건너뛰고, 대문자 A/T/G/C 만을 유효한 문자로 간주하여 연결한다.
 */
string loadReference(const string& file) {
    ifstream fin(file);       // 입력 파일 스트림 열기
    string line, seq;         // line: 파일에서 한 줄 읽기용, seq: 전체 유전체 서열 누적 저장

    while (getline(fin, line)) {
        if (line[0] == '>') continue; // FASTA 주석라인은 무시
        for (char c : line) {
            char uc = toupper(c);     // 소문자일 경우 대문자로 변환
            // A/T/G/C만 유효하게 저장
            if (uc == 'A' || uc == 'T' || uc == 'G' || uc == 'C') 
                seq += uc;
        }
    }
    return seq; // 완성된 참조 서열 반환
}

/**
 * @brief FASTQ 또는 일반 텍스트 형식의 read 목록을 벡터로 불러오는 함수
 * 
 * @param file read 파일 경로 (한 줄에 한 개 read)
 * @return string 벡터 형태의 read 리스트
 * 
 * 주의:
 * - 빈 줄은 무시하고, 각 줄을 하나의 read로 간주한다.
 */
vector<string> loadReads(const string& file) {
    ifstream fin(file);          // 파일 열기
    vector<string> reads;        // 결과를 저장할 read 리스트
    string line;
    while (getline(fin, line)) {
        if (!line.empty())       // 빈 줄은 무시
            reads.push_back(line);
    }
    return reads;                // read 리스트 반환
}

/**
 * @brief 두 문자열 a, b의 mismatch(불일치 문자 수)를 계산하는 함수
 * 
 * @param a 문자열 a (예: read)
 * @param b 문자열 b (예: reference 서열에서 잘라온 부분)
 * @return 불일치 개수 (int)
 * 
 * 주의:
 * - a, b는 같은 길이여야 한다.
 * - 문자열 비교는 위치 기준으로 순차적으로 이루어진다.
 */
int countMismatch(const string& a, const string& b) {
    int count = 0;
    for (int i = 0; i < a.size(); ++i)
        if (a[i] != b[i])        // 동일 위치 문자가 다르면 mismatch 증가
            count++;
    return count;
}

/**
 * @brief 시간 측정 결과를 출력하는 함수
 * 
 * @param label 출력 앞에 붙일 설명 문자열
 * @param start 시작 시간 (chrono 시간 객체)
 * @param end 종료 시간
 * 
 */
void printTime(const string& label, high_resolution_clock::time_point start, high_resolution_clock::time_point end) {
    double duration = chrono::duration_cast<chrono::duration<double>>(end - start).count(); // 초 단위로 변환
    cout << label << " " << duration << "초" << endl;
}
