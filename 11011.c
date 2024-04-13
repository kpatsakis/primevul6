d_lite_wnum0(VALUE self)
{
    get_d1(self);
    return INT2FIX(m_wnum0(dat));
}