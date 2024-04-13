date_s__valid_jd_p(int argc, VALUE *argv, VALUE klass)
{
    VALUE vjd, vsg;
    VALUE argv2[2];

    rb_scan_args(argc, argv, "11", &vjd, &vsg);

    argv2[0] = vjd;
    if (argc < 2)
	argv2[1] = DBL2NUM(GREGORIAN);
    else
	argv2[1] = vsg;

    return valid_jd_sub(2, argv2, klass, 1);
}