f_negative_p(VALUE x)
{
    if (FIXNUM_P(x))
	return f_boolcast(FIX2LONG(x) < 0);
    return rb_funcall(x, '<', 1, INT2FIX(0));
}