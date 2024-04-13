static void record_subpages_vmas(struct page *page, struct vm_area_struct *vma,
				 int refs, struct page **pages,
				 struct vm_area_struct **vmas)
{
	int nr;

	for (nr = 0; nr < refs; nr++) {
		if (likely(pages))
			pages[nr] = mem_map_offset(page, nr);
		if (vmas)
			vmas[nr] = vma;
	}
}