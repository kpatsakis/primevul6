d_lite_nth_kday_p(VALUE self, VALUE n, VALUE k)
{
    int rjd, ns;

    get_d1(self);

    if (NUM2INT(k) != m_wday(dat))
	return Qfalse;

    c_nth_kday_to_jd(m_year(dat), m_mon(dat),
		     NUM2INT(n), NUM2INT(k), m_virtual_sg(dat), /* !=m_sg() */
		     &rjd, &ns);
    if (m_local_jd(dat) != rjd)
	return Qfalse;
    return Qtrue;
}