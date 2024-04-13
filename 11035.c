datetime_s_strptime(int argc, VALUE *argv, VALUE klass)
{
    VALUE str, fmt, sg;

    rb_scan_args(argc, argv, "03", &str, &fmt, &sg);

    switch (argc) {
      case 0:
	str = rb_str_new2("-4712-01-01T00:00:00+00:00");
      case 1:
	fmt = rb_str_new2("%FT%T%z");
      case 2:
	sg = INT2FIX(DEFAULT_SG);
    }

    {
	VALUE argv2[2], hash;

	argv2[0] = str;
	argv2[1] = fmt;
	hash = date_s__strptime(2, argv2, klass);
	return dt_new_by_frags(klass, hash, sg);
    }
}