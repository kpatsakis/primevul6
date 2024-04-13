static void xen_free_ldt(struct desc_struct *ldt, unsigned entries)
{
	const unsigned entries_per_page = PAGE_SIZE / LDT_ENTRY_SIZE;
	int i;

	for (i = 0; i < entries; i += entries_per_page)
		set_aliased_prot(ldt + i, PAGE_KERNEL);
}