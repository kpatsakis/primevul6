qf_range_text(qfline_T *qfp, char_u *buf, int bufsize)
{
    int len;
    vim_snprintf((char *)buf, bufsize, "%ld", qfp->qf_lnum);
    len = (int)STRLEN(buf);

    if (qfp->qf_end_lnum > 0 && qfp->qf_lnum != qfp->qf_end_lnum)
    {
	vim_snprintf((char *)buf + len, bufsize - len,
		"-%ld", qfp->qf_end_lnum);
	len += (int)STRLEN(buf + len);
    }
    if (qfp->qf_col > 0)
    {
	vim_snprintf((char *)buf + len, bufsize - len, " col %d", qfp->qf_col);
	len += (int)STRLEN(buf + len);
	if (qfp->qf_end_col > 0 && qfp->qf_col != qfp->qf_end_col)
	{
	    vim_snprintf((char *)buf + len, bufsize - len,
		    "-%d", qfp->qf_end_col);
	    len += (int)STRLEN(buf + len);
	}
    }
    buf[len] = NUL;
}