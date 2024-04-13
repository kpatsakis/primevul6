static void xen_alloc_ldt(struct desc_struct *ldt, unsigned entries)
{
	const unsigned entries_per_page = PAGE_SIZE / LDT_ENTRY_SIZE;
	int i;

	/*
	 * We need to mark the all aliases of the LDT pages RO.  We
	 * don't need to call vm_flush_aliases(), though, since that's
	 * only responsible for flushing aliases out the TLBs, not the
	 * page tables, and Xen will flush the TLB for us if needed.
	 *
	 * To avoid confusing future readers: none of this is necessary
	 * to load the LDT.  The hypervisor only checks this when the
	 * LDT is faulted in due to subsequent descriptor access.
	 */

	for (i = 0; i < entries; i += entries_per_page)
		set_aliased_prot(ldt + i, PAGE_KERNEL_RO);
}