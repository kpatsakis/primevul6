d_lite_initialize(int argc, VALUE *argv, VALUE self)
{
    VALUE jd, vjd, vdf, sf, vsf, vof, vsg;
    int df, of;
    double sg;

    rb_check_frozen(self);

    rb_scan_args(argc, argv, "05", &vjd, &vdf, &vsf, &vof, &vsg);

    jd = INT2FIX(0);
    df = 0;
    sf = INT2FIX(0);
    of = 0;
    sg = DEFAULT_SG;

    switch (argc) {
      case 5:
	val2sg(vsg, sg);
      case 4:
	val2off(vof, of);
      case 3:
	sf = vsf;
	if (f_lt_p(sf, INT2FIX(0)) ||
	    f_ge_p(sf, INT2FIX(SECOND_IN_NANOSECONDS)))
	    rb_raise(eDateError, "invalid second fraction");
      case 2:
	df = NUM2INT(vdf);
	if (df < 0 || df >= DAY_IN_SECONDS)
	    rb_raise(eDateError, "invalid day fraction");
      case 1:
	jd = vjd;
    }

    {
	VALUE nth;
	int rjd;

	get_d1(self);

	decode_jd(jd, &nth, &rjd);
	if (!df && f_zero_p(sf) && !of) {
	    set_to_simple(self, &dat->s, nth, rjd, sg, 0, 0, 0, HAVE_JD);
	}
	else {
	    if (!complex_dat_p(dat))
		rb_raise(rb_eArgError,
			 "cannot load complex into simple");

	    set_to_complex(self, &dat->c, nth, rjd, df, sf, of, sg,
			   0, 0, 0, 0, 0, 0, HAVE_JD | HAVE_DF);
	}
    }
    return self;
}