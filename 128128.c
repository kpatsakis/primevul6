msg_outtrans_len_attr(char_u *msgstr, int len, int attr)
{
    int		retval = 0;
    char_u	*str = msgstr;
    char_u	*plain_start = msgstr;
    char_u	*s;
    int		mb_l;
    int		c;
    int		save_got_int = got_int;

    // Only quit when got_int was set in here.
    got_int = FALSE;

    // if MSG_HIST flag set, add message to history
    if (attr & MSG_HIST)
    {
	add_msg_hist(str, len, attr);
	attr &= ~MSG_HIST;
    }

    // If the string starts with a composing character first draw a space on
    // which the composing char can be drawn.
    if (enc_utf8 && utf_iscomposing(utf_ptr2char(msgstr)))
	msg_puts_attr(" ", attr);

    /*
     * Go over the string.  Special characters are translated and printed.
     * Normal characters are printed several at a time.
     */
    while (--len >= 0 && !got_int)
    {
	if (enc_utf8)
	    // Don't include composing chars after the end.
	    mb_l = utfc_ptr2len_len(str, len + 1);
	else if (has_mbyte)
	    mb_l = (*mb_ptr2len)(str);
	else
	    mb_l = 1;
	if (has_mbyte && mb_l > 1)
	{
	    c = (*mb_ptr2char)(str);
	    if (vim_isprintc(c))
		// printable multi-byte char: count the cells.
		retval += (*mb_ptr2cells)(str);
	    else
	    {
		// unprintable multi-byte char: print the printable chars so
		// far and the translation of the unprintable char.
		if (str > plain_start)
		    msg_puts_attr_len((char *)plain_start,
					       (int)(str - plain_start), attr);
		plain_start = str + mb_l;
		msg_puts_attr((char *)transchar(c),
					    attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += char2cells(c);
	    }
	    len -= mb_l - 1;
	    str += mb_l;
	}
	else
	{
	    s = transchar_byte(*str);
	    if (s[1] != NUL)
	    {
		// unprintable char: print the printable chars so far and the
		// translation of the unprintable char.
		if (str > plain_start)
		    msg_puts_attr_len((char *)plain_start,
					       (int)(str - plain_start), attr);
		plain_start = str + 1;
		msg_puts_attr((char *)s, attr == 0 ? HL_ATTR(HLF_8) : attr);
		retval += (int)STRLEN(s);
	    }
	    else
		++retval;
	    ++str;
	}
    }

    if (str > plain_start && !got_int)
	// print the printable chars at the end
	msg_puts_attr_len((char *)plain_start, (int)(str - plain_start), attr);

    got_int |= save_got_int;

    return retval;
}