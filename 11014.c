get_s_civil(union DateData *x)
{
    assert(simple_dat_p(x));
    if (!have_civil_p(x)) {
	int y, m, d;

	assert(have_jd_p(x));
	c_jd_to_civil(x->s.jd, s_virtual_sg(x), &y, &m, &d);
	x->s.year = y;
#ifndef USE_PACK
	x->s.mon = m;
	x->s.mday = d;
#else
	x->s.pc = PACK2(m, d);
#endif
	x->s.flags |= HAVE_CIVIL;
    }
}