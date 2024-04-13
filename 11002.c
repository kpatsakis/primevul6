d_lite_new_offset(int argc, VALUE *argv, VALUE self)
{
    VALUE vof;
    int rof;

    rb_scan_args(argc, argv, "01", &vof);

    rof = 0;
    if (argc >= 1)
	val2off(vof, rof);

    return dup_obj_with_new_offset(self, rof);
}