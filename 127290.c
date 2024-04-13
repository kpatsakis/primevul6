static inline void arch_enter_lazy_mmu_mode(void)
{
	PVOP_VCALL0(mmu.lazy_mode.enter);
}