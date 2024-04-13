msg_putchar_attr(int c, int attr)
{
    char_u	buf[MB_MAXBYTES + 1];

    if (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = K_SECOND(c);
	buf[2] = K_THIRD(c);
	buf[3] = NUL;
    }
    else
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    msg_puts_attr((char *)buf, attr);
}