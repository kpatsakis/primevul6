m_cweek(union DateData *x)
{
    int ry, rw, rd;

    c_jd_to_commercial(m_local_jd(x), m_virtual_sg(x), /* !=m_sg() */
		       &ry, &rw, &rd);
    return rw;
}