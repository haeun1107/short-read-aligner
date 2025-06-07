# short-read-aligner
[알고리즘] 팀프로젝트 Suffix Array + Spaced Seed + Mismatch 정렬

본 프로젝트는 Suffix Array, Spaced Seed 필터링, 그리고 Mismatch 처리를 활용한 유전체 정렬 알고리즘 구현입니다.

---

## 📂 프로젝트 파일 설명

- **SA.cpp**  
  최종 알고리즘 구현 코드 (C++)

- **reference.txt**  
  알고리즘이 사용하는 reference 서열 데이터 (길이: 1M bp)

- **reads.txt**  
  reference에서 추출된 랜덤 reads 데이터 (총 1M개, 각 50bp)

- **result.txt**  
  알고리즘 실행 결과가 저장되는 파일

---

## 📸 결과 폴더

실험1 결과, 실험2 결과, 실험3 결과: 알고리즘 실행 결과를 캡처한 이미지 파일들이 포함된 디렉토리입니다. 각 디렉토리에서 알고리즘의 성능 및 정확도 결과를 확인할 수 있습니다.

---

## 🖥️ 실행 방법

```bash
g++ SA.cpp -o aligner
./aligner
