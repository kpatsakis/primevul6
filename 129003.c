static struct page *special_mapping_nopage(struct vm_area_struct *vma,
					   unsigned long address, int *type)
{
	struct page **pages;

	BUG_ON(address < vma->vm_start || address >= vma->vm_end);

	address -= vma->vm_start;
	for (pages = vma->vm_private_data; address > 0 && *pages; ++pages)
		address -= PAGE_SIZE;

	if (*pages) {
		struct page *page = *pages;
		get_page(page);
		return page;
	}

	return NOPAGE_SIGBUS;
}