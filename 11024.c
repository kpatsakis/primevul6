min_trunc(VALUE min, VALUE *fr)
{
    VALUE rmin;

    if (wholenum_p(min)) {
	rmin = to_integer(min);
	*fr = INT2FIX(0);
    }
    else {
	rmin = f_idiv(min, INT2FIX(1));
	*fr = f_mod(min, INT2FIX(1));
	*fr = f_quo(*fr, INT2FIX(1440));
    }
    return rmin;
}