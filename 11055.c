encode_year(VALUE nth, int y, double style,
	    VALUE *ry)
{
    int period;
    VALUE t;

    period = (style < 0) ?
	CM_PERIOD_GCY :
	CM_PERIOD_JCY;
    if (f_zero_p(nth))
	*ry = INT2FIX(y);
    else {
	t = f_mul(INT2FIX(period), nth);
	t = f_add(t, INT2FIX(y));
	*ry = t;
    }
}