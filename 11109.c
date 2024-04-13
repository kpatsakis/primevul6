datetime_s_parse(int argc, VALUE *argv, VALUE klass)
{
    VALUE str, comp, sg, opt;

    rb_scan_args(argc, argv, "03:", &str, &comp, &sg, &opt);
    if (!NIL_P(opt)) argc--;

    switch (argc) {
      case 0:
	str = rb_str_new2("-4712-01-01T00:00:00+00:00");
      case 1:
	comp = Qtrue;
      case 2:
	sg = INT2FIX(DEFAULT_SG);
    }

    {
        int argc2 = 2;
        VALUE argv2[3];
        argv2[0] = str;
        argv2[1] = comp;
        argv2[2] = opt;
        if (!NIL_P(opt)) argc2++;
	VALUE hash = date_s__parse(argc2, argv2, klass);
	return dt_new_by_frags(klass, hash, sg);
    }
}