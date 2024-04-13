static inline unsigned long long paravirt_read_pmc(int counter)
{
	return PVOP_CALL1(u64, cpu.read_pmc, counter);
}