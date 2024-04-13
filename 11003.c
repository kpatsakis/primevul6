rt__valid_ordinal_p(VALUE y, VALUE d, VALUE sg)
{
    VALUE nth, rjd2;
    int ry, rd, rjd, ns;

    if (!valid_ordinal_p(y, NUM2INT(d), NUM2DBL(sg),
			 &nth, &ry,
			 &rd, &rjd,
			 &ns))
	return Qnil;
    encode_jd(nth, rjd, &rjd2);
    return rjd2;
}