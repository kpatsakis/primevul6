d_lite_upto(VALUE self, VALUE max)
{
    VALUE date;

    RETURN_ENUMERATOR(self, 1, &max);

    date = self;
    while (FIX2INT(d_lite_cmp(date, max)) <= 0) {
	rb_yield(date);
	date = d_lite_plus(date, INT2FIX(1));
    }
    return self;
}