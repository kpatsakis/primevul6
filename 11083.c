valid_nth_kday_p(VALUE y, int m, int n, int k, double sg,
		 VALUE *nth, int *ry,
		 int *rm, int *rn, int *rk, int *rjd,
		 int *ns)
{
    double style = guess_style(y, sg);
    int r;

    if (style == 0) {
	int jd;

	r = c_valid_nth_kday_p(FIX2INT(y), m, n, k, sg, rm, rn, rk, &jd, ns);
	if (!r)
	    return 0;
	decode_jd(INT2FIX(jd), nth, rjd);
	if (f_zero_p(*nth))
	    *ry = FIX2INT(y);
	else {
	    VALUE nth2;
	    decode_year(y, *ns ? -1 : +1, &nth2, ry);
	}
    }
    else {
	decode_year(y, style, nth, ry);
	r = c_valid_nth_kday_p(*ry, m, n, k, style, rm, rn, rk, rjd, ns);
    }
    return r;
}