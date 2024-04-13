static void xen_load_gdt(const struct desc_ptr *dtr)
{
	unsigned long va = dtr->address;
	unsigned int size = dtr->size + 1;
	unsigned long pfn, mfn;
	int level;
	pte_t *ptep;
	void *virt;

	/* @size should be at most GDT_SIZE which is smaller than PAGE_SIZE. */
	BUG_ON(size > PAGE_SIZE);
	BUG_ON(va & ~PAGE_MASK);

	/*
	 * The GDT is per-cpu and is in the percpu data area.
	 * That can be virtually mapped, so we need to do a
	 * page-walk to get the underlying MFN for the
	 * hypercall.  The page can also be in the kernel's
	 * linear range, so we need to RO that mapping too.
	 */
	ptep = lookup_address(va, &level);
	BUG_ON(ptep == NULL);

	pfn = pte_pfn(*ptep);
	mfn = pfn_to_mfn(pfn);
	virt = __va(PFN_PHYS(pfn));

	make_lowmem_page_readonly((void *)va);
	make_lowmem_page_readonly(virt);

	if (HYPERVISOR_set_gdt(&mfn, size / sizeof(struct desc_struct)))
		BUG();
}