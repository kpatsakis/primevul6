d_lite_yday(VALUE self)
{
    get_d1(self);
    return INT2FIX(m_yday(dat));
}