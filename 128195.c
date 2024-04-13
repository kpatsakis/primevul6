str2specialbuf(char_u *sp, char_u *buf, int len)
{
    char_u	*s;

    *buf = NUL;
    while (*sp)
    {
	s = str2special(&sp, FALSE);
	if ((int)(STRLEN(s) + STRLEN(buf)) < len)
	    STRCAT(buf, s);
    }
}