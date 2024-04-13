d_lite_initialize_copy(VALUE copy, VALUE date)
{
    rb_check_frozen(copy);

    if (copy == date)
	return copy;
    {
	get_d2(copy, date);
	if (simple_dat_p(bdat)) {
	    if (simple_dat_p(adat)) {
		adat->s = bdat->s;
	    }
	    else {
		adat->c.flags = bdat->s.flags | COMPLEX_DAT;
		adat->c.nth = bdat->s.nth;
		adat->c.jd = bdat->s.jd;
		adat->c.df = 0;
		adat->c.sf = INT2FIX(0);
		adat->c.of = 0;
		adat->c.sg = bdat->s.sg;
		adat->c.year = bdat->s.year;
#ifndef USE_PACK
		adat->c.mon = bdat->s.mon;
		adat->c.mday = bdat->s.mday;
		adat->c.hour = bdat->s.hour;
		adat->c.min = bdat->s.min;
		adat->c.sec = bdat->s.sec;
#else
		adat->c.pc = bdat->s.pc;
#endif
	    }
	}
	else {
	    if (!complex_dat_p(adat))
		rb_raise(rb_eArgError,
			 "cannot load complex into simple");

	    adat->c = bdat->c;
	}
    }
    return copy;
}