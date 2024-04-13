d_lite_inspect_raw(VALUE self)
{
    get_d1(self);
    return mk_inspect_raw(dat, rb_obj_class(self));
}