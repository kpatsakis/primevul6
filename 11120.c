date_s_valid_civil_p(int argc, VALUE *argv, VALUE klass)
{
    VALUE vy, vm, vd, vsg;
    VALUE argv2[4];

    rb_scan_args(argc, argv, "31", &vy, &vm, &vd, &vsg);

    RETURN_FALSE_UNLESS_NUMERIC(vy);
    RETURN_FALSE_UNLESS_NUMERIC(vm);
    RETURN_FALSE_UNLESS_NUMERIC(vd);
    argv2[0] = vy;
    argv2[1] = vm;
    argv2[2] = vd;
    if (argc < 4)
	argv2[3] = INT2FIX(DEFAULT_SG);
    else
	argv2[3] = vsg;

    if (NIL_P(valid_civil_sub(4, argv2, klass, 0)))
	return Qfalse;
    return Qtrue;
}