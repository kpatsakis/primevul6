time_to_datetime(VALUE self)
{
    VALUE y, sf, nth, ret;
    int ry, m, d, h, min, s, of;

    y = f_year(self);
    m = FIX2INT(f_mon(self));
    d = FIX2INT(f_mday(self));

    h = FIX2INT(f_hour(self));
    min = FIX2INT(f_min(self));
    s = FIX2INT(f_sec(self));
    if (s == 60)
	s = 59;

    sf = sec_to_ns(f_subsec(self));
    of = FIX2INT(f_utc_offset(self));

    decode_year(y, -1, &nth, &ry);

    ret = d_complex_new_internal(cDateTime,
				 nth, 0,
				 0, sf,
				 of, DEFAULT_SG,
				 ry, m, d,
				 h, min, s,
				 HAVE_CIVIL | HAVE_TIME);
    {
	get_d1(ret);
	set_sg(dat, DEFAULT_SG);
    }
    return ret;
}