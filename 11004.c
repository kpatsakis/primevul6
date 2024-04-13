date_s_iso8601(int argc, VALUE *argv, VALUE klass)
{
    VALUE str, sg, opt;

    rb_scan_args(argc, argv, "02:", &str, &sg, &opt);
    if (!NIL_P(opt)) argc--;

    switch (argc) {
      case 0:
	str = rb_str_new2("-4712-01-01");
      case 1:
	sg = INT2FIX(DEFAULT_SG);
    }

    {
        int argc2 = 1;
        VALUE argv2[2];
        argv2[0] = str;
        if (!NIL_P(opt)) argv2[argc2++] = opt;
	VALUE hash = date_s__iso8601(argc2, argv2, klass);
	return d_new_by_frags(klass, hash, sg);
    }
}