safe_mul_p(VALUE x, long m)
{
    long ix;

    if (!FIXNUM_P(x))
	return 0;
    ix = FIX2LONG(x);
    if (ix < 0) {
	if (ix <= (FIXNUM_MIN / m))
	    return 0;
    }
    else {
	if (ix >= (FIXNUM_MAX / m))
	    return 0;
    }
    return 1;
}