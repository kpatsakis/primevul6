static inline void mm_populate(unsigned long addr, unsigned long len)
{
	/* Ignore errors */
	(void) __mm_populate(addr, len, 1);
}