d_lite_rshift(VALUE self, VALUE other)
{
    VALUE t, y, nth, rjd2;
    int m, d, rjd;
    double sg;

    get_d1(self);
    t = f_add3(f_mul(m_real_year(dat), INT2FIX(12)),
	       INT2FIX(m_mon(dat) - 1),
	       other);
    if (FIXNUM_P(t)) {
	long it = FIX2LONG(t);
	y = LONG2NUM(DIV(it, 12));
	it = MOD(it, 12);
	m = (int)it + 1;
    }
    else {
	y = f_idiv(t, INT2FIX(12));
	t = f_mod(t, INT2FIX(12));
	m = FIX2INT(t) + 1;
    }
    d = m_mday(dat);
    sg = m_sg(dat);

    while (1) {
	int ry, rm, rd, ns;

	if (valid_civil_p(y, m, d, sg,
			  &nth, &ry,
			  &rm, &rd, &rjd, &ns))
	    break;
	if (--d < 1)
	    rb_raise(eDateError, "invalid date");
    }
    encode_jd(nth, rjd, &rjd2);
    return d_lite_plus(self, f_sub(rjd2, m_real_local_jd(dat)));
}