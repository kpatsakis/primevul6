date_s_valid_jd_p(int argc, VALUE *argv, VALUE klass)
{
    VALUE vjd, vsg;
    VALUE argv2[2];

    rb_scan_args(argc, argv, "11", &vjd, &vsg);

    RETURN_FALSE_UNLESS_NUMERIC(vjd);
    argv2[0] = vjd;
    if (argc < 2)
	argv2[1] = INT2FIX(DEFAULT_SG);
    else
	argv2[1] = vsg;

    if (NIL_P(valid_jd_sub(2, argv2, klass, 0)))
	return Qfalse;
    return Qtrue;
}