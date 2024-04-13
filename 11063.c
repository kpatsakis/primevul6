date_s_new_bang(int argc, VALUE *argv, VALUE klass)
{
    VALUE ajd, of, sg, nth, sf;
    int jd, df, rof;
    double rsg;

    rb_scan_args(argc, argv, "03", &ajd, &of, &sg);

    switch (argc) {
      case 0:
	ajd = INT2FIX(0);
      case 1:
	of = INT2FIX(0);
      case 2:
	sg = INT2FIX(DEFAULT_SG);
    }

    old_to_new(ajd, of, sg,
	       &nth, &jd, &df, &sf, &rof, &rsg);

    if (!df && f_zero_p(sf) && !rof)
	return d_simple_new_internal(klass,
				     nth, jd,
				     rsg,
				     0, 0, 0,
				     HAVE_JD);
    else
	return d_complex_new_internal(klass,
				      nth, jd,
				      df, sf,
				      rof, rsg,
				      0, 0, 0,
				      0, 0, 0,
				      HAVE_JD | HAVE_DF);
}