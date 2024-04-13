date_to_datetime(VALUE self)
{
    get_d1a(self);

    if (simple_dat_p(adat)) {
	VALUE new = d_lite_s_alloc_simple(cDateTime);
	{
	    get_d1b(new);
	    bdat->s = adat->s;
	    return new;
	}
    }
    else {
	VALUE new = d_lite_s_alloc_complex(cDateTime);
	{
	    get_d1b(new);
	    bdat->c = adat->c;
	    bdat->c.df = 0;
	    RB_OBJ_WRITE(new, &bdat->c.sf, INT2FIX(0));
#ifndef USE_PACK
	    bdat->c.hour = 0;
	    bdat->c.min = 0;
	    bdat->c.sec = 0;
#else
	    bdat->c.pc = PACK5(EX_MON(adat->c.pc), EX_MDAY(adat->c.pc),
			       0, 0, 0);
	    bdat->c.flags |= HAVE_DF | HAVE_TIME;
#endif
	    return new;
	}
    }
}