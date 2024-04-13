dt_lite_jisx0301(int argc, VALUE *argv, VALUE self)
{
    long n = 0;

    rb_check_arity(argc, 0, 1);
    if (argc >= 1)
	n = NUM2LONG(argv[0]);

    return rb_str_append(d_lite_jisx0301(self),
			 iso8601_timediv(self, n));
}