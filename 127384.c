static inline void set_debugreg(unsigned long val, int reg)
{
	PVOP_VCALL2(cpu.set_debugreg, reg, val);
}