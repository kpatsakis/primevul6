div_sf(VALUE s, VALUE *f)
{
    VALUE n = sec_to_ns(s);

    if (f)
	*f = f_mod(n, INT2FIX(1));
    return f_floor(n);
}