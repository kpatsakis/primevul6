static inline u64 paravirt_read_msr(unsigned msr)
{
	return PVOP_CALL1(u64, cpu.read_msr, msr);
}