static inline void arch_leave_lazy_mmu_mode(void)
{
	PVOP_VCALL0(mmu.lazy_mode.leave);
}