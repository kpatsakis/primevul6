static inline unsigned long long paravirt_sched_clock(void)
{
	return PVOP_CALL0(unsigned long long, time.sched_clock);
}