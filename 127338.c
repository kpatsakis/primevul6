static inline void load_sp0(unsigned long sp0)
{
	PVOP_VCALL1(cpu.load_sp0, sp0);
}