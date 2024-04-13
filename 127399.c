static __always_inline bool pv_vcpu_is_preempted(long cpu)
{
	return PVOP_CALLEE1(bool, lock.vcpu_is_preempted, cpu);
}