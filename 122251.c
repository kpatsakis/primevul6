static inline void pud_clear(pud_t *pudp)
{
	set_pud(pudp, __pud(0));

	/*
	 * According to Intel App note "TLBs, Paging-Structure Caches,
	 * and Their Invalidation", April 2007, document 317080-001,
	 * section 8.1: in PAE mode we explicitly have to flush the
	 * TLB via cr3 if the top-level pgd is changed...
	 *
	 * Currently all places where pud_clear() is called either have
	 * flush_tlb_mm() followed or don't need TLB flush (x86_64 code or
	 * pud_clear_bad()), so we don't need TLB flush here.
	 */
}