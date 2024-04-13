msg_outtrans_long_len_attr(char_u *longstr, int len, int attr)
{
    int		slen = len;
    int		room;

    room = Columns - msg_col;
    if (len > room && room >= 20)
    {
	slen = (room - 3) / 2;
	msg_outtrans_len_attr(longstr, slen, attr);
	msg_puts_attr("...", HL_ATTR(HLF_8));
    }
    msg_outtrans_len_attr(longstr + len - slen, slen, attr);
}