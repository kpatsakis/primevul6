div_day(VALUE d, VALUE *f)
{
    if (f)
	*f = f_mod(d, INT2FIX(1));
    return f_floor(d);
}