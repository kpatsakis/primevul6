static inline u64 paravirt_read_msr_safe(unsigned msr, int *err)
{
	return PVOP_CALL2(u64, cpu.read_msr_safe, msr, err);
}