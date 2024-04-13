d_lite_cweek(VALUE self)
{
    get_d1(self);
    return INT2FIX(m_cweek(dat));
}