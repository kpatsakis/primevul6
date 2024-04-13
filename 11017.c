d_lite_jisx0301(VALUE self)
{
    char fmtbuf[JISX0301_DATE_SIZE];
    const char *fmt;

    get_d1(self);
    fmt = jisx0301_date_format(fmtbuf, sizeof(fmtbuf),
			       m_real_local_jd(dat),
			       m_real_year(dat));
    return strftimev(fmt, self, set_tmx);
}