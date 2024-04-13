static void __init xen_load_gdt_boot(const struct desc_ptr *dtr)
{
	unsigned long va = dtr->address;
	unsigned int size = dtr->size + 1;
	unsigned long pfn, mfn;
	pte_t pte;

	/* @size should be at most GDT_SIZE which is smaller than PAGE_SIZE. */
	BUG_ON(size > PAGE_SIZE);
	BUG_ON(va & ~PAGE_MASK);

	pfn = virt_to_pfn(va);
	mfn = pfn_to_mfn(pfn);

	pte = pfn_pte(pfn, PAGE_KERNEL_RO);

	if (HYPERVISOR_update_va_mapping((unsigned long)va, pte, 0))
		BUG();

	if (HYPERVISOR_set_gdt(&mfn, size / sizeof(struct desc_struct)))
		BUG();
}