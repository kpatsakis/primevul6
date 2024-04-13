static inline void __flush_tlb_local(void)
{
	PVOP_VCALL0(mmu.flush_tlb_user);
}