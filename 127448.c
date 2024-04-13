static inline void __flush_tlb_one_user(unsigned long addr)
{
	PVOP_VCALL1(mmu.flush_tlb_one_user, addr);
}