static inline void __write_cr4(unsigned long x)
{
	PVOP_VCALL1(cpu.write_cr4, x);
}