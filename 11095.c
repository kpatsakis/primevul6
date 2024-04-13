mk_inspect_raw(union DateData *x, VALUE klass)
{
    char flags[6];

    flags[0] = (x->flags & COMPLEX_DAT) ? 'C' : 'S';
    flags[1] = (x->flags & HAVE_JD)     ? 'j' : '-';
    flags[2] = (x->flags & HAVE_DF)     ? 'd' : '-';
    flags[3] = (x->flags & HAVE_CIVIL)  ? 'c' : '-';
    flags[4] = (x->flags & HAVE_TIME)   ? 't' : '-';
    flags[5] = '\0';

    if (simple_dat_p(x)) {
	return rb_enc_sprintf(rb_usascii_encoding(),
			      "#<%"PRIsVALUE": "
			      "(%+"PRIsVALUE"th,%dj),+0s,%.0fj; "
			      "%dy%dm%dd; %s>",
			      klass,
			      x->s.nth, x->s.jd, x->s.sg,
#ifndef USE_PACK
			      x->s.year, x->s.mon, x->s.mday,
#else
			      x->s.year,
			      EX_MON(x->s.pc), EX_MDAY(x->s.pc),
#endif
			      flags);
    }
    else {
	return rb_enc_sprintf(rb_usascii_encoding(),
			      "#<%"PRIsVALUE": "
			      "(%+"PRIsVALUE"th,%dj,%ds,%+"PRIsVALUE"n),"
			      "%+ds,%.0fj; "
			      "%dy%dm%dd %dh%dm%ds; %s>",
			      klass,
			      x->c.nth, x->c.jd, x->c.df, x->c.sf,
			      x->c.of, x->c.sg,
#ifndef USE_PACK
			      x->c.year, x->c.mon, x->c.mday,
			      x->c.hour, x->c.min, x->c.sec,
#else
			      x->c.year,
			      EX_MON(x->c.pc), EX_MDAY(x->c.pc),
			      EX_HOUR(x->c.pc), EX_MIN(x->c.pc),
			      EX_SEC(x->c.pc),
#endif
			      flags);
    }
}