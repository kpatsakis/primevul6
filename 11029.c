c_valid_gregorian_p(int y, int m, int d, int *rm, int *rd)
{
    int last;

    if (m < 0)
	m += 13;
    if (m < 1 || m > 12)
	return 0;
    last = c_gregorian_last_day_of_month(y, m);
    if (d < 0)
	d = last + d + 1;
    if (d < 1 || d > last)
	return 0;
    *rm = m;
    *rd = d;
    return 1;
}