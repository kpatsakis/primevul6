offset_to_sec(VALUE vof, int *rof)
{
    int try_rational = 1;

  again:
    switch (TYPE(vof)) {
      case T_FIXNUM:
	{
	    long n;

	    n = FIX2LONG(vof);
	    if (n != -1 && n != 0 && n != 1)
		return 0;
	    *rof = (int)n * DAY_IN_SECONDS;
	    return 1;
	}
      case T_FLOAT:
	{
	    double n;

	    n = RFLOAT_VALUE(vof) * DAY_IN_SECONDS;
	    if (n < -DAY_IN_SECONDS || n > DAY_IN_SECONDS)
		return 0;
	    *rof = (int)round(n);
	    if (*rof != n)
		rb_warning("fraction of offset is ignored");
	    return 1;
	}
      default:
	expect_numeric(vof);
	vof = f_to_r(vof);
	if (!k_rational_p(vof)) {
	    if (!try_rational) Check_Type(vof, T_RATIONAL);
	    try_rational = 0;
	    goto again;
	}
	/* fall through */
      case T_RATIONAL:
	{
	    VALUE vs, vn, vd;
	    long n;

	    vs = day_to_sec(vof);

	    if (!k_rational_p(vs)) {
		vn = vs;
		goto rounded;
	    }
	    vn = rb_rational_num(vs);
	    vd = rb_rational_den(vs);

	    if (FIXNUM_P(vn) && FIXNUM_P(vd) && (FIX2LONG(vd) == 1))
		n = FIX2LONG(vn);
	    else {
		vn = f_round(vs);
		if (!f_eqeq_p(vn, vs))
		    rb_warning("fraction of offset is ignored");
	      rounded:
		if (!FIXNUM_P(vn))
		    return 0;
		n = FIX2LONG(vn);
		if (n < -DAY_IN_SECONDS || n > DAY_IN_SECONDS)
		    return 0;
	    }
	    *rof = (int)n;
	    return 1;
	}
      case T_STRING:
	{
	    VALUE vs = date_zone_to_diff(vof);
	    long n;

	    if (!FIXNUM_P(vs))
		return 0;
	    n = FIX2LONG(vs);
	    if (n < -DAY_IN_SECONDS || n > DAY_IN_SECONDS)
		return 0;
	    *rof = (int)n;
	    return 1;
	}
    }
    return 0;
}