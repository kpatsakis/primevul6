save_tc_char(char *bufptr, int c1)
{
    if (is7bits(c1) && isprint(c1)) {
	if (c1 == ':' || c1 == '\\')
	    bufptr = save_char(bufptr, '\\');
	bufptr = save_char(bufptr, c1);
    } else {
	char temp[80];

	if (c1 == (c1 & 0x1f)) {	/* iscntrl() returns T on 255 */
	    _nc_SPRINTF(temp, _nc_SLIMIT(sizeof(temp))
			"%.20s", unctrl((chtype) c1));
	} else {
	    _nc_SPRINTF(temp, _nc_SLIMIT(sizeof(temp))
			"\\%03o", c1);
	}
	bufptr = save_string(bufptr, temp);
    }
    return bufptr;
}