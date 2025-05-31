
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

struct AlignmentResult {
    int score;
    int end_i, end_j;
    std::string aligned_query;
    std::string aligned_ref;
};

AlignmentResult smithWaterman(const std::string& query, const std::string& ref) {
    int m = query.size();
    int n = ref.size();
    int match_score = 2;
    int mismatch_penalty = -1;
    int gap_penalty = -1;

    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
    std::vector<std::vector<int>> traceback(m + 1, std::vector<int>(n + 1, 0));

    int max_score = 0, end_i = 0, end_j = 0;

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            int score_diag = dp[i - 1][j - 1] + (query[i - 1] == ref[j - 1] ? match_score : mismatch_penalty);
            int score_up = dp[i - 1][j] + gap_penalty;
            int score_left = dp[i][j - 1] + gap_penalty;
            dp[i][j] = std::max({0, score_diag, score_up, score_left});

            if (dp[i][j] == score_diag) traceback[i][j] = 1;
            else if (dp[i][j] == score_up) traceback[i][j] = 2;
            else if (dp[i][j] == score_left) traceback[i][j] = 3;

            if (dp[i][j] > max_score) {
                max_score = dp[i][j];
                end_i = i;
                end_j = j;
            }
        }
    }

    std::string align_query = "", align_ref = "";
    int i = end_i, j = end_j;
    while (i > 0 && j > 0 && dp[i][j] > 0) {
        if (traceback[i][j] == 1) {
            align_query = query[i - 1] + align_query;
            align_ref = ref[j - 1] + align_ref;
            --i; --j;
        } else if (traceback[i][j] == 2) {
            align_query = query[i - 1] + align_query;
            align_ref = '-' + align_ref;
            --i;
        } else {
            align_query = '-' + align_query;
            align_ref = ref[j - 1] + align_ref;
            --j;
        }
    }

    AlignmentResult result;
    result.score = max_score;
    result.end_i = end_i;
    result.end_j = end_j;
    result.aligned_query = align_query;
    result.aligned_ref = align_ref;
    return result;
}
