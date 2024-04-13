static inline bool is_pci_p2pdma_page(const struct page *page)
{
	return is_zone_device_page(page) &&
		page->pgmap->type == MEMORY_DEVICE_PCI_P2PDMA;
}