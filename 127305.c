static inline void paravirt_write_msr(unsigned msr,
				      unsigned low, unsigned high)
{
	PVOP_VCALL3(cpu.write_msr, msr, low, high);
}