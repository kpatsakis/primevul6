d_lite_julian_p(VALUE self)
{
    get_d1(self);
    return f_boolcast(m_julian_p(dat));
}