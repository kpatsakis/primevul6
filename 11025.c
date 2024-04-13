d_lite_hash(VALUE self)
{
    st_index_t v, h[4];

    get_d1(self);
    h[0] = m_nth(dat);
    h[1] = m_jd(dat);
    h[2] = m_df(dat);
    h[3] = m_sf(dat);
    v = rb_memhash(h, sizeof(h));
    return ST2FIX(v);
}