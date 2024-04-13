ns_to_sec(VALUE n)
{
    if (FIXNUM_P(n))
	return rb_rational_new2(n, INT2FIX(SECOND_IN_NANOSECONDS));
    return f_quo(n, INT2FIX(SECOND_IN_NANOSECONDS));
}