struct address_space *hugetlb_page_mapping_lock_write(struct page *hpage)
{
	struct address_space *mapping = page_mapping(hpage);

	if (!mapping)
		return mapping;

	if (i_mmap_trylock_write(mapping))
		return mapping;

	return NULL;
}