c_julian_to_yday(int y, int m, int d)
{
    assert(m >= 1 && m <= 12);
    return yeartab[c_julian_leap_p(y) ? 1 : 0][m] + d;
}