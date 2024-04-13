static __always_inline void pv_kick(int cpu)
{
	PVOP_VCALL1(lock.kick, cpu);
}