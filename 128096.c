msg_moremsg(int full)
{
    int		attr;
    char_u	*s = (char_u *)_("-- More --");

    attr = HL_ATTR(HLF_M);
    screen_puts(s, (int)Rows - 1, 0, attr);
    if (full)
	screen_puts((char_u *)
		_(" SPACE/d/j: screen/page/line down, b/u/k: up, q: quit "),
		(int)Rows - 1, vim_strsize(s), attr);
}