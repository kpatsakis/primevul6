spell_check_sps(void)
{
    char_u	*p;
    char_u	*s;
    char_u	buf[MAXPATHL];
    int		f;

    sps_flags = 0;
    sps_limit = 9999;

    for (p = p_sps; *p != NUL; )
    {
	copy_option_part(&p, buf, MAXPATHL, ",");

	f = 0;
	if (VIM_ISDIGIT(*buf))
	{
	    s = buf;
	    sps_limit = getdigits(&s);
	    if (*s != NUL && !VIM_ISDIGIT(*s))
		f = -1;
	}
	else if (STRCMP(buf, "best") == 0)
	    f = SPS_BEST;
	else if (STRCMP(buf, "fast") == 0)
	    f = SPS_FAST;
	else if (STRCMP(buf, "double") == 0)
	    f = SPS_DOUBLE;
	else if (STRNCMP(buf, "expr:", 5) != 0
		&& STRNCMP(buf, "file:", 5) != 0
		&& (STRNCMP(buf, "timeout:", 8) != 0
		    || (!VIM_ISDIGIT(buf[8])
				  && !(buf[8] == '-' && VIM_ISDIGIT(buf[9])))))
	    f = -1;

	if (f == -1 || (sps_flags != 0 && f != 0))
	{
	    sps_flags = SPS_BEST;
	    sps_limit = 9999;
	    return FAIL;
	}
	if (f != 0)
	    sps_flags = f;
    }

    if (sps_flags == 0)
	sps_flags = SPS_BEST;

    return OK;
}