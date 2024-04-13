m_yday(union DateData *x)
{
    int jd, ry, rd;
    double sg;

    jd = m_local_jd(x);
    sg = m_virtual_sg(x); /* !=m_sg() */

    if (m_proleptic_gregorian_p(x) ||
	(jd - sg) > 366)
	return c_gregorian_to_yday(m_year(x), m_mon(x), m_mday(x));
    if (m_proleptic_julian_p(x))
	return c_julian_to_yday(m_year(x), m_mon(x), m_mday(x));
    c_jd_to_ordinal(jd, sg, &ry, &rd);
    return rd;
}