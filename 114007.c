static inline int ok_jpg_extend(const int v, const int t) {
    // Figure F.12
    if (v < (1 << (t - 1))) {
        return v + ((-1) << t) + 1;
    } else {
        return v;
    }
}