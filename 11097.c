d_lite_mon(VALUE self)
{
    get_d1(self);
    return INT2FIX(m_mon(dat));
}