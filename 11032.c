date_s_valid_commercial_p(int argc, VALUE *argv, VALUE klass)
{
    VALUE vy, vw, vd, vsg;
    VALUE argv2[4];

    rb_scan_args(argc, argv, "31", &vy, &vw, &vd, &vsg);

    RETURN_FALSE_UNLESS_NUMERIC(vy);
    RETURN_FALSE_UNLESS_NUMERIC(vw);
    RETURN_FALSE_UNLESS_NUMERIC(vd);
    argv2[0] = vy;
    argv2[1] = vw;
    argv2[2] = vd;
    if (argc < 4)
	argv2[3] = INT2FIX(DEFAULT_SG);
    else
	argv2[3] = vsg;

    if (NIL_P(valid_commercial_sub(4, argv2, klass, 0)))
	return Qfalse;
    return Qtrue;
}