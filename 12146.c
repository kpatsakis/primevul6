qf_fmt_text(char_u *text, char_u *buf, int bufsize)
{
    int		i;
    char_u	*p = text;

    for (i = 0; *p != NUL && i < bufsize - 1; ++i)
    {
	if (*p == '\n')
	{
	    buf[i] = ' ';
	    while (*++p != NUL)
		if (!VIM_ISWHITE(*p) && *p != '\n')
		    break;
	}
	else
	    buf[i] = *p++;
    }
    buf[i] = NUL;
}