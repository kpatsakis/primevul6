static inline void flush_free_hpage_work(struct hstate *h)
{
	if (free_vmemmap_pages_per_hpage(h))
		flush_work(&free_hpage_work);
}