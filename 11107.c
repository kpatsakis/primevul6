valid_nth_kday_sub(int argc, VALUE *argv, VALUE klass, int need_jd)
{
    VALUE nth, y;
    int m, n, k, ry, rm, rn, rk;
    double sg;

    y = argv[0];
    m = NUM2INT(argv[1]);
    n = NUM2INT(argv[2]);
    k = NUM2INT(argv[3]);
    sg = NUM2DBL(argv[4]);

    {
	int rjd, ns;
	VALUE rjd2;

	if (!valid_nth_kday_p(y, m, n, k, sg,
			      &nth, &ry,
			      &rm, &rn, &rk, &rjd,
			      &ns))
	    return Qnil;
	if (!need_jd)
	    return INT2FIX(0); /* dummy */
	encode_jd(nth, rjd, &rjd2);
	return rjd2;
    }
}