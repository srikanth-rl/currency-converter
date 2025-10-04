import sys
from collections import defaultdict

def solve():
    data = list(map(int, sys.stdin.read().strip().split()))
    if not data:
        return
    n = data[0]
    arr = data[1:1+n]
    if len(arr) != n:
        # If the input may have newlines or extra spaces, we already handled; just clamp
        arr = arr[:n]

    if n == 0:
        print(0)
        return

    positions = defaultdict(list)
    for idx, val in enumerate(arr, start=1):  # use 1-based positions for clarity
        positions[val].append(idx)

    max_val = max(arr)

    # Build arcs by pairing k-th s with k-th s+1 in order
    arcs = []  # list of (l, r) intervals with l < r
    base_links = 0
    for s in range(1, max_val):
        L = positions.get(s, [])
        R = positions.get(s + 1, [])
        k = min(len(L), len(R))
        base_links += k
        for i in range(k):
            a = L[i]
            b = R[i]
            if a == b:
                # Same position can't happen for distinct sizes; skip defensively
                continue
            l, r = (a, b) if a < b else (b, a)
            arcs.append((l, r))

    # Count crossings among arcs: intervals [l1, r1] and [l2, r2] cross if they overlap partially
    # i.e., l1 < l2 < r1 < r2 or l2 < l1 < r2 < r1
    crossings = 0
    m = len(arcs)
    for i in range(m):
        l1, r1 = arcs[i]
        for j in range(i + 1, m):
            l2, r2 = arcs[j]
            if (l1 < l2 < r1 < r2) or (l2 < l1 < r2 < r1):
                crossings += 1

    # Total openings = base links (one per s->s+1 link) + extra due to crossings (re-openings)
    total_openings = base_links + crossings
    print(total_openings)

if __name__ == "__main__":
    solve()
