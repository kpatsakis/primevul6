static inline void load_TLS(struct thread_struct *t, unsigned cpu)
{
	PVOP_VCALL2(cpu.load_tls, t, cpu);
}