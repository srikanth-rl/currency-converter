#include <bits/stdc++.h>
using namespace std;

static long long cost_permutation_segment(const vector<int>& a, int l, int r) {
    // a is 0-indexed; compute cost for subarray [l, r] inclusive which is a permutation of 1..len
    int m = r - l + 1;
    vector<int> b(m);
    for (int i = 0; i < m; ++i) b[i] = a[l + i];

    vector<int> L(m, -1), R(m, -1);
    // nearest greater to left
    {
        vector<int> st; st.reserve(m);
        for (int i = 0; i < m; ++i) {
            while (!st.empty() && b[st.back()] < b[i]) st.pop_back();
            if (!st.empty()) L[i] = st.back();
            st.push_back(i);
        }
    }
    // nearest greater to right
    {
        vector<int> st; st.reserve(m);
        for (int i = m - 1; i >= 0; --i) {
            while (!st.empty() && b[st.back()] < b[i]) st.pop_back();
            if (!st.empty()) R[i] = st.back();
            st.push_back(i);
        }
    }

    long long sum = 0;
    for (int i = 0; i < m; ++i) {
        if (L[i] == -1 && R[i] == -1) continue; // root
        int pv;
        if (L[i] == -1) pv = b[R[i]];
        else if (R[i] == -1) pv = b[L[i]];
        else pv = min(b[L[i]], b[R[i]]);
        sum += (pv - b[i]);
    }
    return sum;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n; if (!(cin >> n)) return 0;
    vector<int> a(n);
    for (int i = 0; i < n; ++i) cin >> a[i];

    const long long INF = (1LL<<60);
    vector<long long> dp(n + 1, INF);
    dp[0] = 0;

    // For each starting index, scan rightwards, track if [l..r] is a permutation of 1..len
    for (int l = 0; l < n; ++l) {
        // quick prune: if dp[l] already INF, skip
        if (dp[l] >= INF/2) continue;
        static int seen[501];
        for (int i = 1; i <= 500; ++i) seen[i] = 0;
        bool dup = false;
        int mx = 0;
        for (int r = l; r < n; ++r) {
            int v = a[r];
            if (v >= 1 && v <= 500) {
                if (seen[v]) dup = true;
                else seen[v] = 1;
            } else {
                // given constraints, shouldn't happen, but mark as duplicate to prevent any perm
                dup = true;
            }
            mx = max(mx, v);
            int len = r - l + 1;
            if (!dup && mx == len) {
                // this subarray is a permutation of 1..len
                long long c = cost_permutation_segment(a, l, r);
                dp[r + 1] = min(dp[r + 1], dp[l] + c);
            }
        }
    }

    cout << dp[n] << "\n";
    return 0;
}
