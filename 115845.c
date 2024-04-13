sh4_get_fpscr()
{
	int ret;
	asm volatile ("sts fpscr,%0" : "=r" (ret));
	return ret;
}