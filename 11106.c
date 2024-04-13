date_s_today(int argc, VALUE *argv, VALUE klass)
{
    VALUE vsg, nth, ret;
    double sg;
    time_t t;
    struct tm tm;
    int y, ry, m, d;

    rb_scan_args(argc, argv, "01", &vsg);

    if (argc < 1)
	sg = DEFAULT_SG;
    else
	val2sg(vsg, sg);

    if (time(&t) == -1)
	rb_sys_fail("time");
    tzset();
    if (!localtime_r(&t, &tm))
	rb_sys_fail("localtime");

    y = tm.tm_year + 1900;
    m = tm.tm_mon + 1;
    d = tm.tm_mday;

    decode_year(INT2FIX(y), -1, &nth, &ry);

    ret = d_simple_new_internal(klass,
				nth, 0,
				GREGORIAN,
				ry, m, d,
				HAVE_CIVIL);
    {
	get_d1(ret);
	set_sg(dat, sg);
    }
    return ret;
}