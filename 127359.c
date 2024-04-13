static inline void arch_flush_lazy_mmu_mode(void)
{
	PVOP_VCALL0(mmu.lazy_mode.flush);
}