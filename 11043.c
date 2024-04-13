valid_commercial_p(VALUE y, int w, int d, double sg,
		   VALUE *nth, int *ry,
		   int *rw, int *rd, int *rjd,
		   int *ns)
{
    double style = guess_style(y, sg);
    int r;

    if (style == 0) {
	int jd;

	r = c_valid_commercial_p(FIX2INT(y), w, d, sg, rw, rd, &jd, ns);
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
	r = c_valid_commercial_p(*ry, w, d, style, rw, rd, rjd, ns);
    }
    return r;
}