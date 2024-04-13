rt__valid_civil_p(VALUE y, VALUE m, VALUE d, VALUE sg)
{
    VALUE nth, rjd2;
    int ry, rm, rd, rjd, ns;

    if (!valid_civil_p(y, NUM2INT(m), NUM2INT(d), NUM2DBL(sg),
		       &nth, &ry,
		       &rm, &rd, &rjd,
		       &ns))
	return Qnil;
    encode_jd(nth, rjd, &rjd2);
    return rjd2;
}