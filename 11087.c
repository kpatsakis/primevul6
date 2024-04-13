date_s_jd(int argc, VALUE *argv, VALUE klass)
{
    VALUE vjd, vsg, jd, fr, fr2, ret;
    double sg;

    rb_scan_args(argc, argv, "02", &vjd, &vsg);

    jd = INT2FIX(0);
    fr2 = INT2FIX(0);
    sg = DEFAULT_SG;

    switch (argc) {
      case 2:
	val2sg(vsg, sg);
      case 1:
        check_numeric(vjd, "jd");
	num2num_with_frac(jd, positive_inf);
    }

    {
	VALUE nth;
	int rjd;

	decode_jd(jd, &nth, &rjd);
	ret = d_simple_new_internal(klass,
				    nth, rjd,
				    sg,
				    0, 0, 0,
				    HAVE_JD);
    }
    add_frac();
    return ret;
}