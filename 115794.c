sh4_change_fpscr(int off, int on)
{
	int b = sh4_get_fpscr();
	off = ~off;
	off |=   0x00180000;
	on  &= ~ 0x00180000;
	b &= off;
	b |= on;
	sh4_put_fpscr(b);
	__fpscr_values[0] &= off;
	__fpscr_values[0] |= on;
	__fpscr_values[1] &= off;
	__fpscr_values[1] |= on;
}