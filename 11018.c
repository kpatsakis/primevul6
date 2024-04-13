valid_civil_sub(int argc, VALUE *argv, VALUE klass, int need_jd)
{
    VALUE nth, y;
    int m, d, ry, rm, rd;
    double sg;

    y = argv[0];
    m = NUM2INT(argv[1]);
    d = NUM2INT(argv[2]);
    sg = NUM2DBL(argv[3]);

    valid_sg(sg);

    if (!need_jd && (guess_style(y, sg) < 0)) {
	if (!valid_gregorian_p(y, m, d,
			       &nth, &ry,
			       &rm, &rd))
	    return Qnil;
	return INT2FIX(0); /* dummy */
    }
    else {
	int rjd, ns;
	VALUE rjd2;

	if (!valid_civil_p(y, m, d, sg,
			   &nth, &ry,
			   &rm, &rd, &rjd,
			   &ns))
	    return Qnil;
	if (!need_jd)
	    return INT2FIX(0); /* dummy */
	encode_jd(nth, rjd, &rjd2);
	return rjd2;
    }
}