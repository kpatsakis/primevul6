datetime_s_jd(int argc, VALUE *argv, VALUE klass)
{
    VALUE vjd, vh, vmin, vs, vof, vsg, jd, fr, fr2, ret;
    int h, min, s, rof;
    double sg;

    rb_scan_args(argc, argv, "06", &vjd, &vh, &vmin, &vs, &vof, &vsg);

    jd = INT2FIX(0);

    h = min = s = 0;
    fr2 = INT2FIX(0);
    rof = 0;
    sg = DEFAULT_SG;

    switch (argc) {
      case 6:
	val2sg(vsg, sg);
      case 5:
	val2off(vof, rof);
      case 4:
        check_numeric(vs, "second");
	num2int_with_frac(s, positive_inf);
      case 3:
        check_numeric(vmin, "minute");
	num2int_with_frac(min, 3);
      case 2:
        check_numeric(vh, "hour");
	num2int_with_frac(h, 2);
      case 1:
        check_numeric(vjd, "jd");
	num2num_with_frac(jd, 1);
    }

    {
	VALUE nth;
	int rh, rmin, rs, rjd, rjd2;

	if (!c_valid_time_p(h, min, s, &rh, &rmin, &rs))
	    rb_raise(eDateError, "invalid date");
	canon24oc();

	decode_jd(jd, &nth, &rjd);
	rjd2 = jd_local_to_utc(rjd,
			       time_to_df(rh, rmin, rs),
			       rof);

	ret = d_complex_new_internal(klass,
				     nth, rjd2,
				     0, INT2FIX(0),
				     rof, sg,
				     0, 0, 0,
				     rh, rmin, rs,
				     HAVE_JD | HAVE_TIME);
    }
    add_frac();
    return ret;
}