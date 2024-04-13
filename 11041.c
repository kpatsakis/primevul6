mk_ary_of_str(long len, const char *a[])
{
    VALUE o;
    long i;

    o = rb_ary_new2(len);
    for (i = 0; i < len; i++) {
	VALUE e;

	if (!a[i])
	    e = Qnil;
	else {
	    e = rb_usascii_str_new2(a[i]);
	    rb_obj_freeze(e);
	}
	rb_ary_push(o, e);
    }
    rb_obj_freeze(o);
    return o;
}