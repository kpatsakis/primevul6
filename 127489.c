static inline void write_cr0(unsigned long x)
{
	PVOP_VCALL1(cpu.write_cr0, x);
}