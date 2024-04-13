datetime_initialize(int argc, VALUE *argv, VALUE self)
{
    VALUE vy, vm, vd, vh, vmin, vs, vof, vsg, y, fr, fr2, ret;
    int m, d, h, min, s, rof;
    double sg;
    struct ComplexDateData *dat = rb_check_typeddata(self, &d_lite_type);

    if (!complex_dat_p(dat)) {
	rb_raise(rb_eTypeError, "DateTime expected");
    }

    rb_scan_args(argc, argv, "08", &vy, &vm, &vd, &vh, &vmin, &vs, &vof, &vsg);

    y = INT2FIX(-4712);
    m = 1;
    d = 1;

    h = min = s = 0;
    fr2 = INT2FIX(0);
    rof = 0;
    sg = DEFAULT_SG;

    switch (argc) {
      case 8:
	val2sg(vsg, sg);
      case 7:
	val2off(vof, rof);
      case 6:
        check_numeric(vs, "second");
	num2int_with_frac(s, positive_inf);
      case 5:
        check_numeric(vmin, "minute");
	num2int_with_frac(min, 5);
      case 4:
        check_numeric(vh, "hour");
	num2int_with_frac(h, 4);
      case 3:
        check_numeric(vd, "day");
	num2int_with_frac(d, 3);
      case 2:
        check_numeric(vm, "month");
	m = NUM2INT(vm);
      case 1:
        check_numeric(vy, "year");
	y = vy;
    }

    if (guess_style(y, sg) < 0) {
	VALUE nth;
	int ry, rm, rd, rh, rmin, rs;

	if (!valid_gregorian_p(y, m, d,
			       &nth, &ry,
			       &rm, &rd))
	    rb_raise(eDateError, "invalid date");
	if (!c_valid_time_p(h, min, s, &rh, &rmin, &rs))
	    rb_raise(eDateError, "invalid date");
	canon24oc();

	set_to_complex(self, dat,
		       nth, 0,
		       0, INT2FIX(0),
		       rof, sg,
		       ry, rm, rd,
		       rh, rmin, rs,
		       HAVE_CIVIL | HAVE_TIME);
    }
    else {
	VALUE nth;
	int ry, rm, rd, rh, rmin, rs, rjd, rjd2, ns;

	if (!valid_civil_p(y, m, d, sg,
			   &nth, &ry,
			   &rm, &rd, &rjd,
			   &ns))
	    rb_raise(eDateError, "invalid date");
	if (!c_valid_time_p(h, min, s, &rh, &rmin, &rs))
	    rb_raise(eDateError, "invalid date");
	canon24oc();

	rjd2 = jd_local_to_utc(rjd,
			       time_to_df(rh, rmin, rs),
			       rof);

	set_to_complex(self, dat,
		       nth, rjd2,
		       0, INT2FIX(0),
		       rof, sg,
		       ry, rm, rd,
		       rh, rmin, rs,
		       HAVE_JD | HAVE_CIVIL | HAVE_TIME);
    }
    ret = self;
    add_frac();
    return ret;
}