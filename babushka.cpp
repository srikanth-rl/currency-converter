#include <bits/stdc++.h>
using namespace std;

// We model two-layer DP:
// 1) valid[l][r]: whether subarray a[l..r] forms a valid set: values are 1..mx with no duplicates and mx = len
// 2) dpG[l][r]: minimal openings to merge a[l..r] into one group (assuming it is a set when used by partition)
// Merge cost for concatenating two groups L and R (contiguous):
//   Let s_top = min(max(L), max(R)), and large = side with larger max.
//   Cost consists of:
//   - Opening each distinct size in large that is > s_top (to place small under it). That's popcount(large_mask >> (s_top+1)).
//   - If large contains any value < s_top, let t = max value in large that is < s_top.
//       We must open each level in small that lies in (t, s_top] to interleave correctly: popcount(small_mask & bits(t+1..s_top)).
//       Additionally, if small contains any value <= t, we must open size t once more to tuck them under: +1.
//   - Otherwise (no value < s_top in large), no extra openings from small beyond large-openings.
// 3) Partition DP over valid segments to minimize total.

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if(!(cin >> n)) return 0;
    vector<int> a(n);
    for(int i=0;i<n;++i) cin >> a[i];

    const int MAXV = 500; // sizes in [1,500]

    // Precompute masks and maxima and validity
    vector<vector<unsigned long long>> mask(n, vector<unsigned long long>(n, 0));
    // We need bits up to 500; use an array of 8 x 64-bit chunks (512 bits)
    const int CH = 8; // 8*64=512
    vector<vector<array<unsigned long long, CH>>> masks(n, vector<array<unsigned long long, CH>>(n));
    auto setbit = [&](array<unsigned long long, CH>& b, int v){
        int idx = v >> 6; int off = v & 63; b[idx] |= (1ULL << off);
    };
    auto orbits = [&](const array<unsigned long long, CH>& a, const array<unsigned long long, CH>& b){
        array<unsigned long long, CH> c{}; for(int i=0;i<CH;++i) c[i]=a[i]|b[i]; return c; };
    auto popc = [&](const array<unsigned long long, CH>& b){
        unsigned cnt=0; for(int i=0;i<CH;++i) cnt += __builtin_popcountll(b[i]); return cnt; };
    auto shift_right = [&](const array<unsigned long long, CH>& b, int s){
        // shift right by s (logical), returns array
        if(s==0) return b;
        array<unsigned long long, CH> out{};
        int whole = s >> 6; int rem = s & 63;
        for(int i=0;i<CH;i++){
            unsigned long long val=0;
            if(i+whole < CH){
                val = b[i+whole] >> rem;
                if(rem && i+whole+1 < CH) val |= (b[i+whole+1] << (64-rem));
            }
            out[i]=val;
        }
        return out;
    };
    auto mask_range_inclusive = [&](int lbit, int rbit){
        // build bitmask with bits [lbit..rbit] set (1-indexed domain allowed). If lbit>rbit => zeros
        array<unsigned long long, CH> out{};
        if(lbit>rbit) return out;
        int li = lbit>>6, lo=lbit&63;
        int ri = rbit>>6, ro=rbit&63;
        if(li==ri){
            unsigned long long m = (~0ULL);
            if(lo>0) m &= (~0ULL) << lo;
            if(ro<63) m &= ((1ULL<<(ro+1))-1);
            out[li]=m;
        } else {
            // first chunk
            unsigned long long fm = (~0ULL);
            if(lo>0) fm &= (~0ULL) << lo;
            out[li]=fm;
            // middle full ones
            for(int i=li+1;i<ri;++i) out[i]=~0ULL;
            // last chunk
            unsigned long long lm = (ro==63? ~0ULL : ((1ULL<<(ro+1))-1));
            out[ri]=lm;
        }
        return out;
    };

    vector<vector<int>> maxV(n, vector<int>(n, 0));
    vector<vector<bool>> valid(n, vector<bool>(n,false));

    for(int l=0;l<n;++l){
        array<unsigned long long, CH> m{};
        int mx=0; int distinct=0;
        for(int r=l;r<n;++r){
            int v=a[r];
            int idx=v>>6, off=v&63;
            unsigned long long before = m[idx];
            if(((before>>off)&1ULL)==0ULL){
                distinct++;
                m[idx] = before | (1ULL<<off);
            }
            mx = max(mx, v);
            masks[l][r]=m;
            maxV[l][r]=mx;
            int len = r-l+1;
            if(mx==len && distinct==len){
                valid[l][r]=true;
            }
        }
    }

    const int INF = 1e9;
    vector<vector<int>> dpG(n, vector<int>(n, 0));

    auto popcount_gt = [&](const array<unsigned long long, CH>& b, int s_top){
        // count bits strictly greater than s_top
        int bit = s_top+1;
        if(bit>500) return 0; // beyond domain
        int li = bit>>6, lo=bit&63;
        unsigned cnt=0;
        if(lo==0){
            for(int i=li;i<CH;++i) cnt += __builtin_popcountll(b[i]);
        } else {
            // mask lower bits in li
            unsigned long long first = b[li] & (~0ULL<<lo);
            cnt += __builtin_popcountll(first);
            for(int i=li+1;i<CH;++i) cnt += __builtin_popcountll(b[i]);
        }
        return (int)cnt;
    };

    auto has_any_lt = [&](const array<unsigned long long, CH>& b, int s_top){
        if(s_top<=1) return false;
        int rb = s_top-1;
        int ri = rb>>6, ro=rb&63;
        for(int i=0;i<ri;i++) if(b[i]) return true;
        if(ri>=0){
            unsigned long long last = b[ri] & ((ro==63)? ~0ULL : ((1ULL<<(ro+1))-1));
            if(last) return true;
        }
        return false;
    };

    auto max_lt = [&](const array<unsigned long long, CH>& b, int s_top){
        // return maximum t < s_top s.t. bit t set; if none, return 0
        if(s_top<=1) return 0;
        int rb = s_top-1;
        int ri = rb>>6, ro=rb&63;
        // check ri downward
        unsigned long long last = b[ri] & ((ro==63)? ~0ULL : ((1ULL<<(ro+1))-1));
        if(last){
            int off = 63-__builtin_clzll(last);
            return (ri<<6) + off;
        }
        for(int i=ri-1;i>=0;--i){
            if(b[i]){
                int off = 63-__builtin_clzll(b[i]);
                return (i<<6) + off;
            }
        }
        return 0;
    };

    auto count_in_range = [&](const array<unsigned long long, CH>& b, int lbit, int rbit){
        if(lbit>rbit) return 0;
        // build range mask and popcount(b & range)
        auto rng = mask_range_inclusive(lbit, rbit);
        unsigned cnt=0;
        for(int i=0;i<CH;++i) cnt += __builtin_popcountll(b[i] & rng[i]);
        return (int)cnt;
    };

    for(int len=2; len<=n; ++len){
        for(int l=0; l+len-1<n; ++l){
            int r=l+len-1;
            int best = INF;
            for(int k=l; k<r; ++k){
                int mxL = maxV[l][k];
                int mxR = maxV[k+1][r];
                if(mxL==mxR) continue;
                const auto &maskL = masks[l][k];
                const auto &maskR = masks[k+1][r];
                const array<unsigned long long, CH> *large, *small;
                int s_top;
                if(mxL>mxR){ large=&maskL; small=&maskR; s_top=mxR; }
                else { large=&maskR; small=&maskL; s_top=mxL; }
                int large_open = popcount_gt(*large, s_top);
                int small_open = 0;
                int extra_open_t = 0;
                if(has_any_lt(*large, s_top)){
                    int t = max_lt(*large, s_top);
                    if(t+1 <= s_top) small_open = count_in_range(*small, t+1, s_top);
                    // if small has any <= t then opening t once more is required
                    if(count_in_range(*small, 1, t) > 0) extra_open_t = 1;
                }
                int merge_cost = large_open + small_open + extra_open_t;
                int cand = dpG[l][k] + dpG[k+1][r] + merge_cost;
                best = min(best, cand);
            }
            dpG[l][r] = best==INF? INF: best;
        }
    }

    vector<int> dp(n+1, INF);
    dp[0]=0;
    for(int i=1;i<=n;++i){
        int best=INF;
        for(int j=0;j<i;++j){
            if(valid[j][i-1]){
                int extra = (j==i-1? 0 : dpG[j][i-1]);
                best = min(best, dp[j]+extra);
            }
        }
        dp[i]=best;
    }

    cout << dp[n] << "\n";
    return 0;
}
