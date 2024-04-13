trunc_string(
    char_u	*s,
    char_u	*buf,
    int		room_in,
    int		buflen)
{
    size_t	room = room_in - 3; // "..." takes 3 chars
    size_t	half;
    size_t	len = 0;
    int		e;
    int		i;
    int		n;

    if (*s == NUL)
    {
	if (buflen > 0)
	    *buf = NUL;
	return;
    }

    if (room_in < 3)
	room = 0;
    half = room / 2;

    // First part: Start of the string.
    for (e = 0; len < half && e < buflen; ++e)
    {
	if (s[e] == NUL)
	{
	    // text fits without truncating!
	    buf[e] = NUL;
	    return;
	}
	n = ptr2cells(s + e);
	if (len + n > half)
	    break;
	len += n;
	buf[e] = s[e];
	if (has_mbyte)
	    for (n = (*mb_ptr2len)(s + e); --n > 0; )
	    {
		if (++e == buflen)
		    break;
		buf[e] = s[e];
	    }
    }

    // Last part: End of the string.
    i = e;
    if (enc_dbcs != 0)
    {
	// For DBCS going backwards in a string is slow, but
	// computing the cell width isn't too slow: go forward
	// until the rest fits.
	n = vim_strsize(s + i);
	while (len + n > room)
	{
	    n -= ptr2cells(s + i);
	    i += (*mb_ptr2len)(s + i);
	}
    }
    else if (enc_utf8)
    {
	// For UTF-8 we can go backwards easily.
	half = i = (int)STRLEN(s);
	for (;;)
	{
	    do
		half = half - utf_head_off(s, s + half - 1) - 1;
	    while (half > 0 && utf_iscomposing(utf_ptr2char(s + half)));
	    n = ptr2cells(s + half);
	    if (len + n > room || half == 0)
		break;
	    len += n;
	    i = (int)half;
	}
    }
    else
    {
	for (i = (int)STRLEN(s);
		   i - 1 >= 0 && len + (n = ptr2cells(s + i - 1)) <= room; --i)
	    len += n;
    }


    if (i <= e + 3)
    {
	// text fits without truncating
	if (s != buf)
	{
	    len = STRLEN(s);
	    if (len >= (size_t)buflen)
		len = buflen - 1;
	    len = len - e + 1;
	    if (len < 1)
		buf[e - 1] = NUL;
	    else
		mch_memmove(buf + e, s + e, len);
	}
    }
    else if (e + 3 < buflen)
    {
	// set the middle and copy the last part
	mch_memmove(buf + e, "...", (size_t)3);
	len = STRLEN(s + i) + 1;
	if (len >= (size_t)buflen - e - 3)
	    len = buflen - e - 3 - 1;
	mch_memmove(buf + e + 3, s + i, len);
	buf[e + 3 + len - 1] = NUL;
    }
    else
    {
	// can't fit in the "...", just truncate it
	buf[e - 1] = NUL;
    }
}