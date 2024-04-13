d_lite_next_day(int argc, VALUE *argv, VALUE self)
{
    VALUE n;

    rb_scan_args(argc, argv, "01", &n);
    if (argc < 1)
	n = INT2FIX(1);
    return d_lite_plus(self, n);
}