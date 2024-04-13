static inline u64 paravirt_steal_clock(int cpu)
{
	return PVOP_CALL1(u64, time.steal_clock, cpu);
}