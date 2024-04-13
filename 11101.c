c_find_ldom(int y, int m, double sg, int *rjd, int *ns)
{
    int i, rm, rd;

    for (i = 0; i < 30; i++)
	if (c_valid_civil_p(y, m, 31 - i, sg, &rm, &rd, rjd, ns))
	    return 1;
    return 0;
}