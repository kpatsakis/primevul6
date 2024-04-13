sec_to_day(VALUE s)
{
    if (FIXNUM_P(s))
	return rb_rational_new2(s, INT2FIX(DAY_IN_SECONDS));
    return f_quo(s, INT2FIX(DAY_IN_SECONDS));
}