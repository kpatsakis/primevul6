static inline void __flush_tlb_others(const struct cpumask *cpumask,
				      const struct flush_tlb_info *info)
{
	PVOP_VCALL2(mmu.flush_tlb_others, cpumask, info);
}