date_s_httpdate(int argc, VALUE *argv, VALUE klass)
{
    VALUE str, sg, opt;

    rb_scan_args(argc, argv, "02:", &str, &sg, &opt);

    switch (argc) {
      case 0:
	str = rb_str_new2("Mon, 01 Jan -4712 00:00:00 GMT");
      case 1:
	sg = INT2FIX(DEFAULT_SG);
    }

    {
        int argc2 = 1;
        VALUE argv2[2];
        argv2[0] = str;
        if (!NIL_P(opt)) argv2[argc2++] = opt;
	VALUE hash = date_s__httpdate(argc2, argv2, klass);
	return d_new_by_frags(klass, hash, sg);
    }
}