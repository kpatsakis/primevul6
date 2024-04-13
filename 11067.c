minus_dd(VALUE self, VALUE other)
{
    get_d2(self, other);

    {
	int d, df;
	VALUE n, sf, r;

	n = f_sub(m_nth(adat), m_nth(bdat));
	d = m_jd(adat) - m_jd(bdat);
	df = m_df(adat) - m_df(bdat);
	sf = f_sub(m_sf(adat), m_sf(bdat));
	canonicalize_jd(n, d);

	if (df < 0) {
	    d -= 1;
	    df += DAY_IN_SECONDS;
	}
	else if (df >= DAY_IN_SECONDS) {
	    d += 1;
	    df -= DAY_IN_SECONDS;
	}

	if (f_lt_p(sf, INT2FIX(0))) {
	    df -= 1;
	    sf = f_add(sf, INT2FIX(SECOND_IN_NANOSECONDS));
	}
	else if (f_ge_p(sf, INT2FIX(SECOND_IN_NANOSECONDS))) {
	    df += 1;
	    sf = f_sub(sf, INT2FIX(SECOND_IN_NANOSECONDS));
	}

	if (f_zero_p(n))
	    r = INT2FIX(0);
	else
	    r = f_mul(n, INT2FIX(CM_PERIOD));

	if (d)
	    r = f_add(r, rb_rational_new1(INT2FIX(d)));
	if (df)
	    r = f_add(r, isec_to_day(df));
	if (f_nonzero_p(sf))
	    r = f_add(r, ns_to_day(sf));

	if (RB_TYPE_P(r, T_RATIONAL))
	    return r;
	return rb_rational_new1(r);
    }
}