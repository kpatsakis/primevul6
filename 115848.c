sh4_put_fpscr(int nv)
{
	asm volatile ("lds %0,fpscr" : : "r" (nv));
}