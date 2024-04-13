f_cmp(VALUE x, VALUE y)
{
    if (FIXNUM_P(x) && FIXNUM_P(y)) {
	long c = FIX2LONG(x) - FIX2LONG(y);
	if (c > 0)
	    return 1;
	else if (c < 0)
	    return -1;
	return 0;
    }
    return rb_cmpint(rb_funcallv(x, id_cmp, 1, &y), x, y);
}