c_valid_nth_kday_p(int y, int m, int n, int k, double sg,
		   int *rm, int *rn, int *rk, int *rjd, int *ns)
{
    int ns2, ry2, rm2, rn2, rk2;

    if (k < 0)
	k += 7;
    if (n < 0) {
	int t, ny, nm, rjd2;

	t = y * 12 + m;
	ny = DIV(t, 12);
	nm = MOD(t, 12) + 1;

	c_nth_kday_to_jd(ny, nm, 1, k, sg, &rjd2, &ns2);
	c_jd_to_nth_kday(rjd2 + n * 7, sg, &ry2, &rm2, &rn2, &rk2);
	if (ry2 != y || rm2 != m)
	    return 0;
	n = rn2;
    }
    c_nth_kday_to_jd(y, m, n, k, sg, rjd, ns);
    c_jd_to_nth_kday(*rjd, sg, &ry2, rm, rn, rk);
    if (y != ry2 || m != *rm || n != *rn || k != *rk)
	return 0;
    return 1;
}