c_virtual_sg(union DateData *x)
{
    if (isinf(x->c.sg))
	return x->c.sg;
    if (f_zero_p(x->c.nth))
	return x->c.sg;
    else if (f_negative_p(x->c.nth))
	return positive_inf;
    return negative_inf;
}