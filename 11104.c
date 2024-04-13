f_eqeq_p(VALUE x, VALUE y)
{
    if (FIXNUM_P(x) && FIXNUM_P(y))
	return f_boolcast(FIX2LONG(x) == FIX2LONG(y));
    return rb_funcall(x, id_eqeq_p, 1, y);
}