valid_commercial_sub(int argc, VALUE *argv, VALUE klass, int need_jd)
{
    VALUE nth, y;
    int w, d, ry, rw, rd;
    double sg;

    y = argv[0];
    w = NUM2INT(argv[1]);
    d = NUM2INT(argv[2]);
    sg = NUM2DBL(argv[3]);

    valid_sg(sg);

    {
	int rjd, ns;
	VALUE rjd2;

	if (!valid_commercial_p(y, w, d, sg,
				&nth, &ry,
				&rw, &rd, &rjd,
				&ns))
	    return Qnil;
	if (!need_jd)
	    return INT2FIX(0); /* dummy */
	encode_jd(nth, rjd, &rjd2);
	return rjd2;
    }
}