date_s__valid_nth_kday_p(int argc, VALUE *argv, VALUE klass)
{
    VALUE vy, vm, vn, vk, vsg;
    VALUE argv2[5];

    rb_scan_args(argc, argv, "41", &vy, &vm, &vn, &vk, &vsg);

    argv2[0] = vy;
    argv2[1] = vm;
    argv2[2] = vn;
    argv2[3] = vk;
    if (argc < 5)
	argv2[4] = DBL2NUM(GREGORIAN);
    else
	argv2[4] = vsg;

    return valid_nth_kday_sub(5, argv2, klass, 1);
}