interpolate_alpha(long long now, long long t1, long long t2, long long t3,
                  long long t4, int a1, int a2, int a3)
{
    int a;
    double cf;

    if (now < t1) {
        a = a1;
    } else if (now < t2) {
        cf = ((double) (now - t1)) / (t2 - t1);
        a = a1 * (1 - cf) + a2 * cf;
    } else if (now < t3) {
        a = a2;
    } else if (now < t4) {
        cf = ((double) (now - t3)) / (t4 - t3);
        a = a2 * (1 - cf) + a3 * cf;
    } else {                    // now >= t4
        a = a3;
    }

    return a;
}