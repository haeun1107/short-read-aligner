import random

# 설정
N = 100_000_000
M = 20_000_000
L = 50
stride = 5

# Reference 시퀀스 생성
reference = ''.join(random.choices('ACGT', k=N))
with open('reference_10.txt', 'w') as f:
    for i in range(0, len(reference), 100):
        f.write(reference[i:i+100] + '\n')

# Reads 생성
reads = []
max_start = N - L
for i in range(0, M * stride, stride):
    if i > max_start:
        break  # reference 범위를 넘지 않게

    read = list(reference[i:i+L])
    if len(read) < L:
        continue  # L보다 짧은 read는 건너뜀

    if random.random() < 0.1:
        for _ in range(random.randint(1, 2)):
            pos = random.randint(0, L - 1)
            if pos < len(read):  # 안전 체크
                original = read[pos]
                options = ['A', 'C', 'G', 'T']
                options.remove(original)
                read[pos] = random.choice(options)
    reads.append(''.join(read))

with open('reads_20.txt', 'w') as f:
    for r in reads:
        f.write(r + '\n')
