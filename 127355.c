static inline int paravirt_write_msr_safe(unsigned msr,
					  unsigned low, unsigned high)
{
	return PVOP_CALL3(int, cpu.write_msr_safe, msr, low, high);
}