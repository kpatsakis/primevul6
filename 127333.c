static inline void __flush_tlb_global(void)
{
	PVOP_VCALL0(mmu.flush_tlb_kernel);
}