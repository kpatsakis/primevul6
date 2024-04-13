static inline void paravirt_tlb_remove_table(struct mmu_gather *tlb, void *table)
{
	PVOP_VCALL2(mmu.tlb_remove_table, tlb, table);
}