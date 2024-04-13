static void set_aliased_prot(void *v, pgprot_t prot)
{
	int level;
	pte_t *ptep;
	pte_t pte;
	unsigned long pfn;
	struct page *page;
	unsigned char dummy;

	ptep = lookup_address((unsigned long)v, &level);
	BUG_ON(ptep == NULL);

	pfn = pte_pfn(*ptep);
	page = pfn_to_page(pfn);

	pte = pfn_pte(pfn, prot);

	/*
	 * Careful: update_va_mapping() will fail if the virtual address
	 * we're poking isn't populated in the page tables.  We don't
	 * need to worry about the direct map (that's always in the page
	 * tables), but we need to be careful about vmap space.  In
	 * particular, the top level page table can lazily propagate
	 * entries between processes, so if we've switched mms since we
	 * vmapped the target in the first place, we might not have the
	 * top-level page table entry populated.
	 *
	 * We disable preemption because we want the same mm active when
	 * we probe the target and when we issue the hypercall.  We'll
	 * have the same nominal mm, but if we're a kernel thread, lazy
	 * mm dropping could change our pgd.
	 *
	 * Out of an abundance of caution, this uses __get_user() to fault
	 * in the target address just in case there's some obscure case
	 * in which the target address isn't readable.
	 */

	preempt_disable();

	copy_from_kernel_nofault(&dummy, v, 1);

	if (HYPERVISOR_update_va_mapping((unsigned long)v, pte, 0))
		BUG();

	if (!PageHighMem(page)) {
		void *av = __va(PFN_PHYS(pfn));

		if (av != v)
			if (HYPERVISOR_update_va_mapping((unsigned long)av, pte, 0))
				BUG();
	} else
		kmap_flush_unused();

	preempt_enable();
}