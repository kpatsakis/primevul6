bcd_expression(const char *str)
{
    /* leave this non-const for HPUX */
    static char fmt[] = "%%p%c%%{10}%%/%%{16}%%*%%p%c%%{10}%%m%%+";
    int len = 0;
    char ch1, ch2;

    if (sscanf(str, fmt, &ch1, &ch2) == 2
	&& isdigit(UChar(ch1))
	&& isdigit(UChar(ch2))
	&& (ch1 == ch2)) {
	len = 28;
#ifndef NDEBUG
	{
	    char buffer[80];
	    int tst;
	    _nc_SPRINTF(buffer, _nc_SLIMIT(sizeof(buffer)) fmt, ch1, ch2);
	    tst = strlen(buffer) - 1;
	    assert(len == tst);
	}
#endif
    }
    return len;
}