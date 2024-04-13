static inline unsigned long paravirt_get_debugreg(int reg)
{
	return PVOP_CALL1(unsigned long, cpu.get_debugreg, reg);
}