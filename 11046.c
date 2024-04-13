f_zero_p(VALUE x)
{
    switch (TYPE(x)) {
      case T_FIXNUM:
	return f_boolcast(FIX2LONG(x) == 0);
      case T_BIGNUM:
	return Qfalse;
      case T_RATIONAL:
	{
	    VALUE num = rb_rational_num(x);
	    return f_boolcast(FIXNUM_P(num) && FIX2LONG(num) == 0);
	}
    }
    return rb_funcall(x, id_eqeq_p, 1, INT2FIX(0));
}