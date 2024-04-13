del_from_showcmd(int len)
{
    int	    old_len;

    if (!p_sc)
	return;

    old_len = (int)STRLEN(showcmd_buf);
    if (len > old_len)
	len = old_len;
    showcmd_buf[old_len - len] = NUL;

    if (!char_avail())
	display_showcmd();
}