msg_outtrans_one(char_u *p, int attr)
{
    int		l;

    if (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
    {
	msg_outtrans_len_attr(p, l, attr);
	return p + l;
    }
    msg_puts_attr((char *)transchar_byte(*p), attr);
    return p + 1;
}