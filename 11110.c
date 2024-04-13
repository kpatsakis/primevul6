d_lite_mjd(VALUE self)
{
    get_d1(self);
    return f_sub(m_real_local_jd(dat), INT2FIX(2400001));
}