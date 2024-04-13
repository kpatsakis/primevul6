c_weeknum_to_jd(int y, int w, int d, int f, double sg, int *rjd, int *ns)
{
    int rjd2, ns2;

    c_find_fdoy(y, sg, &rjd2, &ns2);
    rjd2 += 6;
    *rjd = (rjd2 - MOD(((rjd2 - f) + 1), 7) - 7) + 7 * w + d;
    *ns = (*rjd < sg) ? 0 : 1;
}