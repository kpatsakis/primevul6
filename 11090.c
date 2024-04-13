valid_ordinal_sub(int argc, VALUE *argv, VALUE klass, int need_jd)
{
    VALUE nth, y;
    int d, ry, rd;
    double sg;

    y = argv[0];
    d = NUM2INT(argv[1]);
    sg = NUM2DBL(argv[2]);

    valid_sg(sg);

    {
	int rjd, ns;
	VALUE rjd2;

	if (!valid_ordinal_p(y, d, sg,
			     &nth, &ry,
			     &rd, &rjd,
			     &ns))
	    return Qnil;
	if (!need_jd)
	    return INT2FIX(0); /* dummy */
	encode_jd(nth, rjd, &rjd2);
	return rjd2;
    }
}