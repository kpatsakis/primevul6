m_nth(union DateData *x)
{
    if (simple_dat_p(x))
	return x->s.nth;
    else {
	get_c_civil(x);
	return x->c.nth;
    }
}