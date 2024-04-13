m_real_jd(union DateData *x)
{
    VALUE nth, rjd;
    int jd;

    nth = m_nth(x);
    jd = m_jd(x);

    encode_jd(nth, jd, &rjd);
    return rjd;
}