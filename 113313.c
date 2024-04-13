static inline bool put_devmap_managed_page(struct page *page)
{
	if (!static_branch_unlikely(&devmap_managed_key))
		return false;
	if (!is_zone_device_page(page))
		return false;
	switch (page->pgmap->type) {
	case MEMORY_DEVICE_PRIVATE:
	case MEMORY_DEVICE_PUBLIC:
	case MEMORY_DEVICE_FS_DAX:
		__put_devmap_managed_page(page);
		return true;
	default:
		break;
	}
	return false;
}