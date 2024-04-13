static inline unsigned long read_cr0(void)
{
	return PVOP_CALL0(unsigned long, cpu.read_cr0);
}