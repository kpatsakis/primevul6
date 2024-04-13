tmx_m_msecs(union DateData *x)
{
    VALUE s, sf;

    s = sec_to_ms(tmx_m_secs(x));
    if (simple_dat_p(x))
	return s;
    sf = m_sf(x);
    if (f_nonzero_p(sf))
	s = f_add(s, f_div(sf, INT2FIX(MILLISECOND_IN_NANOSECONDS)));
    return s;
}